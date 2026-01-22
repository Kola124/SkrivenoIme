/*****************************************************************
/*  File:   rsRenderSystemDX.h                                  *
/*  Desc:   Direct3D rendering system                            *
/*  Author: Silver, Copyright (C) GSC Game World                 *
/*  Date:   Feb 2002                                             *
/*****************************************************************/
#ifndef __D3DRENDERSYSTEM_H__
#define __D3DRENDERSYSTEM_H__

#include "IRenderSystem.h"
#include "rsSettings.h"

const int c_MaxAdapters				= 3;
const int c_MaxAdapterDevices		= 5;
const int c_MaxDeviceDisplayModes	= 150;
const int c_MaxDisplayFormatsInMode	= 10;

/*****************************************************************************/
/*	Class:	D3DModeInfo
/*	Desc:	Direct3D display mode description
/*****************************************************************************/
class D3DModeInfo
{
public:
	void			Dump( FILE* fp );

protected:
	int			width;
	int			height;
	int			refreshRate;
	D3DFORMAT	format;
	DWORD		behavior;
	D3DFORMAT	depthStencil;

private:
	friend class D3DRenderSystem;
	friend class D3DDeviceInfo;
}; // class D3DModeInfo

/*****************************************************************************/
/*	Class:	D3DDeviceInfo
/*	Desc:	Direct3D device description
/*****************************************************************************/
class D3DDeviceInfo
{
public:
	D3DDeviceInfo() : nModes(0) {}

	void			Dump( FILE* fp );
	D3DModeInfo*	FindMode( int width, int height );

protected:
	D3DDEVTYPE		devType;
    DXCaps			caps;
	const char*		strDesc;
	bool			canDoWindowed;

    int				nModes;
	D3DModeInfo		modes[c_MaxDeviceDisplayModes];

private:
	friend class D3DRenderSystem;
	friend class D3DAdapterInfo;
}; // class D3DDeviceInfo

/*****************************************************************************/
/*	Class:	D3DAdapterInfo
/*	Desc:	Direct3D adapter info
/*****************************************************************************/
class D3DAdapterInfo
{
public:
	D3DAdapterInfo() : nDevices(0) {}
	
	void			Dump( FILE* fp );
	D3DDeviceInfo*	FindDevice( D3DDEVTYPE devType );

protected:
#ifdef _DX9
    D3DADAPTER_IDENTIFIER9		adapterID;
#else
    D3DADAPTER_IDENTIFIER8		adapterID;
#endif
    D3DDISPLAYMODE				desktopDisplayMode;
    int							nDevices;
    D3DDeviceInfo				devices[c_MaxAdapterDevices];

private:
	friend class D3DRenderSystem;
}; // class D3DAdapterInfo

class IniFile;
class Shader;
class Input;

/*****************************************************************
/*	Class:	D3DRenderSystem
/*	Desc:	Direct3D rendering system
/*****************************************************************/
class D3DRenderSystem : public IRenderSystem
{
public:
	D3DRenderSystem();
	~D3DRenderSystem();

	void __stdcall Init( HWND hWnd, bool subclassWnd = true );
	void __stdcall ShutDown();
	void __stdcall SetViewMatrix( const Matrix4D& vmatr );
	void __stdcall SetProjectionMatrix( const Matrix4D& pmatr );
	void __stdcall SetWorldMatrix( const Matrix4D& wmatr );
	void __stdcall SetTextureMatrix( const Matrix4D& tmatr, int stage = 0 );
	
	const Matrix4D& __stdcall GetWorldMatrix();
	

	void __stdcall ClearDeviceTarget( DWORD color = 0x00000000 );
	void __stdcall ClearDeviceZBuffer();
	void __stdcall ClearDeviceStencil();
	void __stdcall PresentBackBuffer( const RECT* rect = 0 );
	void __stdcall BeginScene();
	void __stdcall EndScene();
	void __stdcall Draw( BaseMesh& bm, int firstPoly, int numPoly, int firstVert, int numVert );
	void __stdcall Draw( BaseMesh& bm );
	void __stdcall DrawPrim( BaseMesh& bm, int firstPoly, int numPoly, int firstVert, int numVert );
	void __stdcall DrawPrim( BaseMesh& bm );

	void __stdcall SetCursorBitmap( const char* bmpName );
	void __stdcall SetCursorPosition( int x, int y, bool drawNow = false );

	void __stdcall SetTexture				( int texID, int stage = 0		);
	int __stdcall GetTexture( int stage = 0 );

	void __stdcall SaveTexture			( int texID, const char* fname			);
	BYTE* __stdcall LockTexBits			( int texID, int& pitch, int level = 0	);
	void  __stdcall UnlockTexBits		( int texID, int level = 0				);
	
	void __stdcall SetViewPort( const ViewPort& vp );
	void __stdcall SetRenderTarget( int texID );

	void* __stdcall DbgGetDevice();
	void __stdcall OnFrame();

	void __stdcall Dump( const char* fname = 0 );

	void __stdcall SetCurrentShader( int shaderID );
	int	 __stdcall GetShaderID( const char* shaderName );
	bool __stdcall IsShaderValid( const char* shaderName );

	int	 __stdcall GetTextureID( const char* texName );
	int	 __stdcall GetTextureID( const char* texName, const TextureDescr& td );
	int	 __stdcall CreateTexture( const char* texName, const TextureDescr& td, 
									TextureClient* client = 0 );

	void __stdcall SetRootDir( const char* rootDir );

	bool __stdcall SetScreenProperties( const ScreenProp& prop );	
	ScreenProp __stdcall GetScreenProperties();	

	void __stdcall BindToTexture( BaseMesh* drawn );
	void __stdcall CopyTexture( int destID, int srcID );
	void __stdcall CopyTexture( int destID, int srcID, 
							RECT* rectList, int nRect, POINT* ptList = 0 );

	bool __stdcall RecompileAllShaders();
	bool __stdcall ReloadAllTextures();
	void __stdcall SetTextureFactor( DWORD tfactor );

	int	 __stdcall GetVBufferID();

	void __stdcall GetClientSize( int& width, int& height );
	int	__stdcall GetTextureSizeBytes( int texID );
	int  __stdcall EstimateTexMemSizeMB();

	void __stdcall dbgDumpShader( int shaderID, FILE* fp );
	
protected:
	DXSurface*				GetTexSurface( int texID );
	bool					ResetDevice();
	
	HRESULT					InitDeviceObjects();
	HRESULT					RestoreDeviceObjects();
	HRESULT					InvalidateDeviceObjects();

	void					AdjustWindowPos( int x, int y, int w, int h );
	bool					InitD3D();
	bool					InitDeviceD3D();
	bool					ShutDeviceD3D();

	void					GetPresentParameters( D3DPRESENT_PARAMETERS& presParm );
	void					BuildDeviceList();
	void					DumpDeviceList( FILE* fp );
	void					RestoreDesktopDisplayMode();

private:
	static DXAPI*			pD3D;
	static DXDevice*		pDevice;
	
	DXSurface*				curRenderTargetSurf; 
	DXSurface*				backBufferSurf;
	DXSurface*				depthStencilSurf;

	char					rootDirectory[_MAX_PATH];
	
	HWND					hRenderWindow;
	
	int						renderTarget;
	ViewPort				curViewPort;

	int						vbufID;

	Settings				settings;
	Matrix4D				curWorldMatr;

	static ScreenProp		screenProp;
	static DEVMODE			dmDesktop;

	int						defShader;		// default shader
	int						curShader;		// current shader

	TextureCache			textureCache;
	ShaderCache				shaderCache;
	PrimitiveCache			primitiveCache;

	D3DAdapterInfo			adapterList[c_MaxAdapters];
	int						nAdapters;
	D3DDeviceInfo*			curDeviceInfo;

}; // class D3DRenderSystem

#endif // __D3DRENDERSYSTEM_H__