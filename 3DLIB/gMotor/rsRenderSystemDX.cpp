/*****************************************************************
/*  File:   rsRenderSystemDX.cpp                                 *
/*  Desc:   Direct3D rendering system implementation             *
/*  Author: Silver, Copyright (C) GSC Game World                 *
/*  Date:   Feb 2002                                             *
/*****************************************************************/
#include "stdafx.h"

#include "direct.h"
#include "rsDX.h"

#include "kStatistics.h"
#include "kStrUtil.h"
#ifdef _DX9
IDirect3D9*								D3DRenderSystem::pD3D;
#else
IDirect3D8*								D3DRenderSystem::pD3D;
#endif
DXDevice*								D3DRenderSystem::pDevice;
ScreenProp								D3DRenderSystem::screenProp;
DEVMODE									D3DRenderSystem::dmDesktop;

int dbgGetRefCount( IUnknown* iface )
{
	int nRef = iface->AddRef() - 1;
	iface->Release();
	return nRef;
} // dbgGetRefCount

//  O== PUBLIC INTERFACE ========================================O
D3DRenderSystem::D3DRenderSystem() 
{
	curWorldMatr.setIdentity();

	renderTarget		= 0;
	curShader			= 0;

	backBufferSurf		= NULL;
	depthStencilSurf	= NULL;
	curRenderTargetSurf = NULL;

	nAdapters			= 0;
}

D3DRenderSystem::~D3DRenderSystem()
{
	ShutDown();
}

void __stdcall 	
D3DRenderSystem::SetViewMatrix( const Matrix4D& vmatr )
{
	DX_CHK( pDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX*)&vmatr ) );
}
	
void __stdcall 	
D3DRenderSystem::SetProjectionMatrix( const Matrix4D& pmatr )
{
	DX_CHK( pDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)&pmatr ) );
}
	
void __stdcall 		
D3DRenderSystem::SetWorldMatrix( const Matrix4D& wmatr )
{
	curWorldMatr = wmatr;
	DX_CHK( pDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&wmatr ) );
	INC_COUNTER( WMatr, 1 );
}

void __stdcall 
D3DRenderSystem::SetTextureMatrix( const Matrix4D& tmatr, int stage )
{
	assert( stage >= 0 && stage < 8 );
	DX_CHK( pDevice->SetTransform( (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + stage), 
									(D3DMATRIX*)&tmatr ) );
}

const Matrix4D&
D3DRenderSystem::GetWorldMatrix()
{
	return curWorldMatr;
}
	
void __stdcall 		
D3DRenderSystem::ClearDeviceTarget( DWORD color )
{
	DX_CHK( pDevice->Clear( 0, 
							NULL, 
							D3DCLEAR_TARGET, 
							color, 
							1.0, 
							0 ) );
}

void __stdcall 		
D3DRenderSystem::ClearDeviceZBuffer()
{
	DX_CHK( pDevice->Clear( 0, 
							NULL, 
							D3DCLEAR_ZBUFFER, 
							0, 
							1.0, 
							0 ) );
}

void __stdcall 		
D3DRenderSystem::ClearDeviceStencil()
{
	DX_CHK( pDevice->Clear( 0, 
							NULL, 
							D3DCLEAR_STENCIL, 
							0, 
							1.0, 
							0 ) );
}
	
void __stdcall 
D3DRenderSystem::PresentBackBuffer( const RECT* rect )
{
	assert( pDevice );
	HRESULT hres = pDevice->TestCooperativeLevel();
	
	if (hres == D3DERR_DEVICELOST)
	{
		return;
	}

	if (hres == D3DERR_DEVICENOTRESET)
    {
		Log.Warning( "Device is lost. Resetting device and recreating/reloading"
						" all device-dependent resources..." );
		ResetDevice();
		return;
    }
	
	DX_CHK( pDevice->Present( rect, rect, NULL, NULL ) );
} // D3DRenderSystem::PresentBackBuffer
	
void __stdcall 		
D3DRenderSystem::BeginScene()
{
	DX_CHK( pDevice->BeginScene() );
}

void __stdcall 		
D3DRenderSystem::EndScene()
{
	DX_CHK( pDevice->EndScene() );
}

void __stdcall 		
D3DRenderSystem::OnFrame()
{
	PresentBackBuffer();
	Stats::OnFrame();
	RC::OnFrame();
}

int __stdcall D3DRenderSystem::GetVBufferID()
{
	if (vbufID != -1) return vbufID;
	//  create vbuffer texture
	TextureDescr vbTD;
	vbTD.setValues( screenProp.width,screenProp.width/* screenProp.height*/, screenProp.colorFormat, 
					mpVRAM, 1, tuRenderTarget );
	vbufID = CreateTexture( "VBuffer", vbTD );
	assert( vbufID != -1 );
	return vbufID;
} // D3DRenderSystem::GetVBufferID

void __stdcall 
D3DRenderSystem::DrawPrim( BaseMesh& bm, int firstPoly, int numPoly, 
							int firstVert, int numVert )
{
	DX_CHK( pDevice->BeginScene() );
	primitiveCache.Draw( bm, firstPoly, numPoly, firstVert, numVert );
	DX_CHK( pDevice->EndScene() );
} // D3DRenderSystem::DrawPrim

void __stdcall 
D3DRenderSystem::DrawPrim( BaseMesh& bm )
{
	DX_CHK( pDevice->BeginScene() );
	primitiveCache.Draw( bm );
	DX_CHK( pDevice->EndScene() );
} // D3DRenderSystem::DrawPrim

/*---------------------------------------------------------------------------*/
/*	Func:	D3DRenderSystem::Draw
/*	Desc:	Draws mesh part
/*---------------------------------------------------------------------------*/
void __stdcall 
D3DRenderSystem::Draw(	BaseMesh& bm, 
						int firstPoly, int numPoly,
						int firstVert, int numVert )
{
	DX_CHK( pDevice->BeginScene() );
	textureCache.SetTexture( bm.getTexture( 0 ), 0 );
	textureCache.SetTexture( bm.getTexture( 1 ), 1 );
	shaderCache.SetShader( bm.getShader() ); 

	primitiveCache.Draw( bm, firstPoly, numPoly, firstVert, numVert );

	DX_CHK( pDevice->EndScene() );
} // D3DRenderSystem::Draw
	
void __stdcall 		
D3DRenderSystem::Draw( BaseMesh& bm )
{
	DX_CHK( pDevice->BeginScene() );
	textureCache.SetTexture( bm.getTexture( 0 ), 0 );
	textureCache.SetTexture( bm.getTexture( 1 ), 1 );
	shaderCache.SetShader( bm.getShader() ); 

	primitiveCache.Draw( bm );
	DX_CHK( pDevice->EndScene() );
} // D3DRenderSystem::Draw

void __stdcall 
D3DRenderSystem::SetCursorPosition( int x, int y, bool drawNow )
{
	assert( false );
	return;
	assert( pDevice );
	DWORD flags = drawNow ? D3DCURSOR_IMMEDIATE_UPDATE : 0;
	DX_CHK( pDevice->ShowCursor( TRUE ) );
	pDevice->SetCursorPosition( x, y, flags );
} // D3DRenderSystem::SetCursorPosition

void __stdcall 
D3DRenderSystem::SetCursorBitmap( const char* bmpName )
{return;
	assert( pDevice );
	TextureDescr td;
	td.setValues( -1, -1, cfARGB8888, mpManaged, 1 );
	int bmpTex = GetTextureID( bmpName, td ); 
	DX_CHK( pDevice->SetCursorProperties( 0, 0, GetTexSurface( bmpTex ) ) );
	SaveTexture( bmpTex, "c:\\dumps\\tipaCursor.dds" );
} // D3DRenderSystem::SetCursorBitmap
#ifdef _DX9
IDirect3DSurface9* D3DRenderSystem::GetTexSurface( int texID )
#else
IDirect3DSurface8* D3DRenderSystem::GetTexSurface( int texID )
#endif
{
	return textureCache.GetTextureSurface( texID, 0 );
} // D3DRenderSystem::GetTexSurface

void __stdcall 
D3DRenderSystem::CopyTexture( int destID, int srcID )
{
	assert( destID != 0 && srcID != 0 );
	textureCache.CopyTexture( destID, srcID );
}  // D3DRenderSystem::CopyTexture

void __stdcall 
D3DRenderSystem::CopyTexture( int destID, int srcID, 
							 RECT* rectList, int nRect, POINT* ptList )
{
	if (destID != 0 && srcID != 0)
	{
		textureCache.CopyTexture( destID, srcID, rectList, nRect, ptList );
		return;
	}
	//  copy to the back buffer
	if (destID == 0)
	{
		textureCache.CopyTexture( backBufferSurf, srcID, rectList, nRect, ptList );
		return;
	}
} // D3DRenderSystem::CopyTexture

int __stdcall 
D3DRenderSystem::GetTexture( int stage )
{
	return textureCache.GetCurrentTexture( stage );
}  // D3DRenderSystem::GetTexture

void __stdcall 	
D3DRenderSystem::SetTexture( int texID, int stage )
{
	textureCache.SetTexture( texID, stage );
} // D3DRenderSystem::SetTexture
	
int __stdcall 	
D3DRenderSystem::GetTextureID( const char* texName, const TextureDescr& td )
{
	char fname[_MAX_PATH];
	strcpy( fname, rootDirectory );
	strcat( fname, "\\textures\\" );
	strcat( fname, texName );

	if (td.getColFmt() == cfBackBufferCompatible)
	{
		TextureDescr tdex( td );
		tdex.setColFmt( screenProp.colorFormat );
		return textureCache.GetTextureID( fname, tdex );
	}
	return textureCache.GetTextureID( fname, td );
} // D3DRenderSystem::LoadTexture

int	 __stdcall 
D3DRenderSystem::CreateTexture( const char* texName, const TextureDescr& td, TextureClient* client )
{
	if (td.getColFmt() == cfBackBufferCompatible)
	{
		TextureDescr tdex( td );
		tdex.setColFmt( screenProp.colorFormat );
		return textureCache.CreateTexture( texName, tdex, client );
	}
	return textureCache.CreateTexture( texName, td, client );
} // D3DRenderSystem::CreateTexture

void __stdcall 	 
D3DRenderSystem::SaveTexture( int texID, const char* fname )
{
	textureCache.SaveTexture( texID, fname );
} // D3DRenderSystem::SaveTexture

int __stdcall 		
D3DRenderSystem::GetTextureSizeBytes( int texID )
{
	return textureCache.GetTextureSizeBytes( texID );
}  // D3DRenderSystem::GetTextureSizeBytes

int  __stdcall 
D3DRenderSystem::EstimateTexMemSizeMB()
{
	massert( pDevice, "Device pointer is NULL." );
	UINT res = pDevice->GetAvailableTextureMem();
	return res;
} // D3DRenderSystem::EstimateTexMemSizeMB

BYTE*  __stdcall 	
D3DRenderSystem::LockTexBits( int texID, int& pitch, int level )
{
	return textureCache.LockTexture( texID, pitch, level );
}  // D3DRenderSystem::LockTexBits

void __stdcall 	
D3DRenderSystem::UnlockTexBits( int texID, int level )
{
	textureCache.UnlockTexture( texID, level );
} // D3DRenderSystem::UnlockTexBits

void __stdcall 	 
D3DRenderSystem::GetClientSize( int& width, int& height )
{
	assert( false );
}  // D3DRenderSystem::GetClientSize

void __stdcall 	
D3DRenderSystem::SetViewPort( const ViewPort& vp )
{
#ifdef _DX9
	pDevice->SetViewport( (D3DVIEWPORT9*)&vp );
#else
    pDevice->SetViewport( (D3DVIEWPORT8*)&vp );
#endif
	curViewPort = vp;
} // D3DRenderSystem::SetViewPort

void __stdcall 	
D3DRenderSystem::SetRenderTarget( int texID )
{
	if (renderTarget == texID)
	//  render target already has been set
	{
		return;
	}

	renderTarget	= texID;
	
	if (renderTarget == 0)
	{
		DX_CHK( pDevice->SetRenderTarget( backBufferSurf, depthStencilSurf ) );
		return;
	}

	curRenderTargetSurf = textureCache.GetTextureSurface( renderTarget );
	DX_CHK( pDevice->SetRenderTarget( curRenderTargetSurf, 0 ) );
} // D3DRenderSystem::SetRenderTarget

void* __stdcall 
D3DRenderSystem::DbgGetDevice()
{
	return (void*)pDevice;
}
	
void __stdcall D3DRenderSystem::dbgDumpShader( int shaderID, FILE* fp )
{
	shaderCache.DumpShader( shaderID, fp );
}

bool D3DRenderSystem::InitD3D()
{
#ifdef _DX9
	pD3D = Direct3DCreate9( D3D_SDK_VERSION );
#else
    pD3D = Direct3DCreate8( D3D_SDK_VERSION );
#endif
	massert( pD3D != NULL, "Failed to create DirectX device" );
	BuildDeviceList();

	//  pick default device
	curDeviceInfo = adapterList[0].FindDevice( D3DDEVTYPE_HAL );
	massert( curDeviceInfo, "No render device available." );

	//FILE* fp = fopen( "c:\\dumps\\deviceList.txt", "wt" );
	//DumpDeviceList( fp );
	//fclose( fp );
	return true;
} // D3DRenderSystem::InitD3D

bool D3DRenderSystem::ShutDeviceD3D()
{
	SetTexture( 0, 0 );
	SetTexture( 0, 1 );
	SetCurrentShader( 0 );

	SAFE_RELEASE( backBufferSurf );
	SAFE_RELEASE( depthStencilSurf );

	shaderCache.Shut();
	primitiveCache.Shut();
	textureCache.Shut();

#ifndef _DEBUG
	SAFE_RELEASE( pDevice );
#else
	FORCE_RELEASE( pDevice );
#endif // _DEBUG	
	return true;
} // D3DRenderSystem::ShutDeviceD3D

bool D3DRenderSystem::InitDeviceD3D()
{
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
	
	BOOL bWindowed = (screenProp.fullScreen == false);
    d3dpp.Windowed = bWindowed;

	if (bWindowed) 
	{
		d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
	}
	else
	{
		d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
	}
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.BackBufferFormat		 = ColorFormatG2DX( screenProp.colorFormat );
    d3dpp.BackBufferCount        = 1;
    d3dpp.hDeviceWindow          = hRenderWindow;

	DWORD displayModeFlags = 0;
	DEVMODE devMode;
	ZeroMemory( &devMode, sizeof( devMode ) );
	devMode.dmSize          = sizeof( DEVMODE );
	devMode.dmPelsWidth     = screenProp.width;
	devMode.dmPelsHeight    = screenProp.height;
	devMode.dmDisplayFrequency = screenProp.refreshRate;
	devMode.dmFields        = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

	if (screenProp.fullScreen) displayModeFlags |= CDS_FULLSCREEN;

	int devModeRes = ChangeDisplaySettings( &devMode, displayModeFlags );
	if (devModeRes != DISP_CHANGE_SUCCESSFUL)
	{
		Log.Error( GetDispChangeErrorDesc( devModeRes ) );
	}

	//if (screenProp.fullScreen || !screenProp.wholeScreenWnd) AdjustWindowPos( 0, 0, screenProp.width, screenProp.height );
	ShowWindow( hRenderWindow, SW_SHOW );

	int wndWidth  = screenProp.width;
	int wndHeight = screenProp.height;

	if (!screenProp.wholeScreenWnd && !screenProp.fullScreen) 
	{
		RECT rctClient;
		GetClientRect( hRenderWindow, &rctClient );
		wndWidth  = rctClient.right - rctClient.left;
		wndHeight = rctClient.bottom - rctClient.top;
	}

	d3dpp.BackBufferWidth  = wndWidth;
    d3dpp.BackBufferHeight = wndHeight;
	DWORD flags = D3DCREATE_MIXED_VERTEXPROCESSING;

	
	//if (settings.hardwareTnL) flags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	//	else flags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	

	if (settings.pureDevice) flags |= D3DCREATE_PUREDEVICE;
	
	D3DDEVTYPE devType = settings.softwareRendering ? D3DDEVTYPE_REF : D3DDEVTYPE_HAL;

    // Create the D3DDevice
    DX_CHK( pD3D->CreateDevice( settings.adapterOrdinal, 
								devType, 
								hRenderWindow,
                                flags,
                                &d3dpp, 
								&pDevice ) );

	DX_CHK( pDevice->ShowCursor( TRUE ) );
	return true;
} // D3DRenderSystem::InitDXDevice

void D3DRenderSystem::AdjustWindowPos( int x, int y, int w, int h )
{
	SetWindowLong( hRenderWindow, GWL_STYLE, WS_POPUP ); 
	SetWindowPos( hRenderWindow, HWND_NOTOPMOST, x, y, w, h, 0 );
} // D3DRenderSystem::AdjustWindowPos

void __stdcall 		
D3DRenderSystem::Init( HWND hWnd, bool subclassWnd )
{
	massert( hWnd != 0, "D3D Render System - No window handle." );

	_getcwd( rootDirectory, _MAX_PATH );
	hRenderWindow = hWnd;

	Log.Info( "Initializing D3D Render System..." );

	InitD3D();

	// Get the current desktop display mode, so we can set up a back
    // buffer of the same format
    D3DDISPLAYMODE d3ddm;
    assert( pD3D );
	DX_CHK( pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) );

	assert( curDeviceInfo );
	D3DModeInfo* modeInfo = curDeviceInfo->FindMode( d3ddm.Width, d3ddm.Height );
	massert( modeInfo, "Current display mode is not supported." );

	screenProp.width		= d3ddm.Width;
    screenProp.height		= d3ddm.Height;
    screenProp.refreshRate	= modeInfo->refreshRate;
    screenProp.colorFormat	= ColorFormatDX2G( d3ddm.Format );

	//  get current desktop display mode in order to restore it at shutdown
	EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &dmDesktop );

	RECT rct;
	GetClientRect( hWnd, &rct );
	int clWidth = rct.right - rct.left;
	int clHeight = rct.bottom - rct.top;
	if ((d3ddm.Width == clWidth) && (d3ddm.Height == clHeight))
	{
		screenProp.wholeScreenWnd = true;
	}
	else
	{
		screenProp.wholeScreenWnd = false;
	}

	InitDeviceD3D();	
	InitDeviceObjects();

	Log.Info( "D3D Render System was initialized succesfully." );
} // D3DRenderSystem::Init

HRESULT D3DRenderSystem::InitDeviceObjects()
{
#ifdef _DX9
	DX_CHK( pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBufferSurf ) );
#else
    DX_CHK( pDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &backBufferSurf ) );
#endif
	DX_CHK( pDevice->GetDepthStencilSurface( &depthStencilSurf ) );

	shaderCache		.Init( pDevice );
	primitiveCache	.Init( pDevice );
	textureCache	.Init( pDevice );

	vbufID = -1;
	return S_OK;
} // D3DRenderSystem::InitDeviceObjects

HRESULT D3DRenderSystem::RestoreDeviceObjects()
{
#ifdef _DX9
	DX_CHK( pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBufferSurf ) );
#else
    DX_CHK( pDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &backBufferSurf ) );
#endif
	DX_CHK( pDevice->GetDepthStencilSurface	( &depthStencilSurf ) );
	
	shaderCache		.SetDevice( pDevice );
	primitiveCache	.SetDevice( pDevice );
	textureCache	.SetDevice( pDevice );

	shaderCache		.RestoreDeviceObjects();
	primitiveCache	.RestoreDeviceObjects();
	textureCache	.RestoreDeviceObjects();

	if (vbufID != -1)
	{
		vbufID = -1;
		vbufID = GetVBufferID();
	}

	return S_OK;
} // D3DRenderSystem::RestoreDeviceObjects

void __stdcall 
D3DRenderSystem::SetRootDir( const char* rootDir )
{
	strcpy( rootDirectory, rootDir );
} // D3DRenderSystem::SetRootDir

void __stdcall 	 D3DRenderSystem::ShutDown()
{
	ShutDeviceD3D();
	RestoreDesktopDisplayMode();

#ifndef _DEBUG
	SAFE_RELEASE( pD3D );
#else
	FORCE_RELEASE( pD3D );
#endif // _DEBUG
}//  D3DRenderSystem::ShutDown

void D3DRenderSystem::RestoreDesktopDisplayMode()
{
	int devModeRes = ChangeDisplaySettings( &dmDesktop, 0 );
	if (devModeRes != DISP_CHANGE_SUCCESSFUL)
	{
		Log.Error( "Could not restore desktop display mode: %s", 
							GetDispChangeErrorDesc( devModeRes ) );
	}
} // D3DRenderSystem::RestoreDesktopDisplayMode

void fpmatr( FILE* fp, D3DMATRIX& matr )
{
	fprintf( fp, "\n%8.5f %8.5f %8.5f %8.5f", 
					matr._11, matr._12, matr._13, matr._14 );	
	fprintf( fp, "\n%8.5f %8.5f %8.5f %8.5f", 
					matr._21, matr._22, matr._23, matr._24 );	
	fprintf( fp, "\n%8.5f %8.5f %8.5f %8.5f", 
					matr._31, matr._32, matr._33, matr._34 );	
	fprintf( fp, "\n%8.5f %8.5f %8.5f %8.5f", 
					matr._41, matr._42, matr._43, matr._44 );	
}

void __stdcall 	
D3DRenderSystem::Dump( const char* fname )
{
	FILE* fp = fname ? fopen( fname, "wt" ) : fopen( "c:\\dumps\\DeviceDump.txt", "wt" );
	if (!fp) return;

	fprintf( fp, "--------- CURRENT DEVICE STATE VALUES --------" );
	Shader sha;
	sha.CreateFromCurrentDeviceState( pDevice );
	sha.Dump( fp );
	fprintf( fp, "--------- --------" );

	textureCache.DumpStatus( fp );
	shaderCache.Dump( fp );

	fprintf( fp, "\nTransforms:\n" );
	D3DMATRIX matr;
	pDevice->GetTransform( D3DTS_VIEW, &matr );
	fprintf( fp, "\n\nView, Det=%f:\n", D3DXMatrixfDeterminant( (D3DXMATRIX*)&matr ) );
	fpmatr( fp, matr );
	pDevice->GetTransform( D3DTS_PROJECTION, &matr );
	fprintf( fp, "\n\nProjection, Det=%f:", D3DXMatrixfDeterminant( (D3DXMATRIX*)&matr ) );
	fpmatr( fp, matr );
	fprintf( fp, "\n\nWorld: Det=%f\n", D3DXMatrixfDeterminant( (D3DXMATRIX*)&matr ) );
	pDevice->GetTransform( D3DTS_WORLD, &matr );
	fpmatr( fp, matr );

	// texture matrices
	fprintf( fp, "\n\nTexture0:\n" );
	pDevice->GetTransform( D3DTS_TEXTURE0, &matr );
	fpmatr( fp, matr );
	fprintf( fp, "\n\nTexture1:\n" );
	pDevice->GetTransform( D3DTS_TEXTURE1, &matr );
	fpmatr( fp, matr );
	fprintf( fp, "\n\nTexture2:\n" );
	pDevice->GetTransform( D3DTS_TEXTURE2, &matr );
	fpmatr( fp, matr );
	fprintf( fp, "\n\nTexture3:\n" );
	pDevice->GetTransform( D3DTS_TEXTURE3, &matr );
	fpmatr( fp, matr );
	fprintf( fp, "\n\nTexture4:\n" );
	pDevice->GetTransform( D3DTS_TEXTURE4, &matr );
	fpmatr( fp, matr );
	fprintf( fp, "\n\nTexture5:\n" );
	pDevice->GetTransform( D3DTS_TEXTURE5, &matr );
	fpmatr( fp, matr );
	fprintf( fp, "\n\nTexture6:\n" );
	pDevice->GetTransform( D3DTS_TEXTURE6, &matr );
	fpmatr( fp, matr );
	fprintf( fp, "\n\nTexture7:\n" );
	pDevice->GetTransform( D3DTS_TEXTURE7, &matr );
	fpmatr( fp, matr );
#ifdef _DX9
	D3DVIEWPORT9 vp;
#else
    D3DVIEWPORT8 vp;
#endif
	pDevice->GetViewport( &vp );
	fprintf( fp, "\nViewport: x=%f; y=%f; w=%f; h =%f; minz=%f; maxz=%f\n",
				vp.X, vp.Y, vp.Width, vp.Height, vp.MinZ, vp.MaxZ );
#ifdef _DX9
	IDirect3DSurface9* rt;
#else
    IDirect3DSurface8* rt;
#endif
	pDevice->GetRenderTarget( &rt );
	fprintf( fp, "\nRender target: %X\n", rt );

	fclose( fp );
}
 
void __stdcall 	 
D3DRenderSystem::SetCurrentShader( int shaderID )
{
	bool res = shaderCache.SetShader( shaderID ); 
} // D3DRenderSystem::SetCurrentShader

int __stdcall 		 
D3DRenderSystem::GetTextureID( const char* texName )
{
	char fname[_MAX_PATH];
	strcpy( fname, rootDirectory );
	strcat( fname, "\\textures\\" );
	strcat( fname, texName );

	return textureCache.GetTextureID( fname );
} // D3DRenderSystem::GetTextureID

bool __stdcall 
D3DRenderSystem::IsShaderValid( const char* shaderName )
{
	//return true;
	//  create file name
	char fname[_MAX_PATH];
	sprintf( fname, "%s\\shaders\\%s.sha", rootDirectory, shaderName );

	ID3DXEffect* eff;
	ID3DXBuffer* compileErr;
	HRESULT hres = D3DXCreateEffectFromFile( pDevice, fname, &eff, &compileErr );
	if (hres != S_OK) 
	{
		if (!compileErr)
		{
			Log.Warning( "Could not load shader file %s", fname );
			return false;
		}
		else
		{
			char* errbuf = (char*)compileErr->GetBufferPointer();
			Log.Warning( "Could not compile shader <%s>. Error message: %s", 
							shaderName, errbuf );
			SAFE_RELEASE( compileErr );
			return false;
		}
	}
	hres = eff->SetTechnique( "T0" );
	bool valid = true;
	if (hres != S_OK) valid = false;
	//  Checking if device is capable to set this shader
	if (eff->Validate() != S_OK) valid = false;
	SAFE_RELEASE( eff );
	return valid;
} // D3DRenderSystem::IsShaderValid

int __stdcall 		 
D3DRenderSystem::GetShaderID( const char* shaderName )
{
	return shaderCache.GetShaderID( shaderName );
} // D3DRenderSystem::GetShaderID

bool __stdcall 
D3DRenderSystem::RecompileAllShaders()
{	
	shaderCache.ReloadShaders();
	return true;
} // D3DRenderSystem::RecompileAllShaders

bool __stdcall 
D3DRenderSystem::ReloadAllTextures()
{
	textureCache.ReloadTextures();
	return true;
}

void __stdcall D3DRenderSystem::SetTextureFactor( DWORD tfactor )
{
	DX_CHK( pDevice->SetRenderState( D3DRS_TEXTUREFACTOR, tfactor ) );	
}

void D3DRenderSystem::GetPresentParameters( D3DPRESENT_PARAMETERS& presParm )
{
	if (screenProp.fullScreen)
	{
		presParm.BackBufferWidth	= screenProp.width;
		presParm.BackBufferHeight	= screenProp.height;
	}
	else
	{
		RECT rect;
		assert( hRenderWindow );
		GetClientRect( hRenderWindow, &rect );
		presParm.BackBufferWidth	= rect.right - rect.left;
		presParm.BackBufferHeight	= rect.bottom - rect.top;
	}

	presParm.BackBufferFormat		= ColorFormatG2DX( screenProp.colorFormat );
	presParm.BackBufferCount		= 1;
									
	presParm.MultiSampleType		= D3DMULTISAMPLE_NONE;
									
	presParm.SwapEffect				= D3DSWAPEFFECT_COPY;
	presParm.hDeviceWindow			= hRenderWindow;
	presParm.Windowed				= screenProp.fullScreen ? FALSE : TRUE;
	presParm.EnableAutoDepthStencil = TRUE;
	presParm.AutoDepthStencilFormat = D3DFMT_D16;
	presParm.Flags					= 0;

    presParm.FullScreen_RefreshRateInHz			= screenProp.fullScreen ? D3DPRESENT_RATE_DEFAULT : 0;
    presParm.FullScreen_PresentationInterval	= D3DPRESENT_INTERVAL_DEFAULT;
}

bool D3DRenderSystem::ResetDevice()
{
	InvalidateDeviceObjects();

	D3DPRESENT_PARAMETERS presParm;
	GetPresentParameters( presParm );
	
	//  resetting device
	HRESULT hres = pDevice->Reset( &presParm );
	if (hres != S_OK) 
	{
		return false;
	}

	//  set display fullscreen mode
	DEVMODE devMode;
	ZeroMemory( &devMode, sizeof( devMode ) );
	devMode.dmSize          = sizeof( DEVMODE );

	devMode.dmPelsWidth     = screenProp.width;
	devMode.dmPelsHeight    = screenProp.height;
	devMode.dmFields        = DM_PELSWIDTH | DM_PELSHEIGHT;
	
	DWORD flags = screenProp.fullScreen ? CDS_FULLSCREEN : 0;
	int devModeRes = ChangeDisplaySettings( &devMode, flags );
	if (devModeRes != DISP_CHANGE_SUCCESSFUL)
	{
		assert( false );
		return false;
	}

	//  reinitialize all previously released resources
	RestoreDeviceObjects();

	D3DSURFACE_DESC bbDesc;
	LPDIRECT3DSURFACE8 pBackBuffer;
#ifdef _DX9
    LPDIRECT3DSURFACE9 pBackBuffer;
    pDevice->GetBackBuffer( 0,0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
#else
    pDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
#endif
    pBackBuffer->GetDesc( &bbDesc );
    pBackBuffer->Release();


	DWORD numPasses;
	DX_CHK( pDevice->ValidateDevice( &numPasses ) );	
	return true;
} // D3DRenderSystem::ResetDevice

ScreenProp __stdcall 
D3DRenderSystem::GetScreenProperties()
{
	return screenProp;
} // D3DRenderSystem::GetScreenProperties

/*---------------------------------------------------------------------------*/
/*	Func:	D3DRenderSystem::SetScreenProperties
/*---------------------------------------------------------------------------*/
bool __stdcall 
D3DRenderSystem::SetScreenProperties( const ScreenProp& prop )
{
	//  check if nothing changes
	ScreenProp rProp = prop;
	if (rProp.colorFormat == cfBackBufferCompatible) 
		rProp.colorFormat = screenProp.colorFormat;
	if (rProp.equal( screenProp )) return true;

	screenProp = rProp;

	//  select maximal possible refresh rate
	assert( curDeviceInfo );
	D3DModeInfo* modeInfo = curDeviceInfo->FindMode( screenProp.width, screenProp.height );
	if (!modeInfo)
	{
		Log.Error( "Display mode %dx%d is not supported.", screenProp.width, screenProp.height );
		return false;
	}

	screenProp.refreshRate	= modeInfo->refreshRate;

	if (prop.colorFormat != cfBackBufferCompatible)
	{
		screenProp.colorFormat = prop.colorFormat;
	}
	
	//ResetDevice();
	ShutDeviceD3D();
	InitDeviceD3D();	
	RestoreDeviceObjects();

	return true;
} // D3DRenderSystem::SetScreenProperties		

//  O== INTERNAL METHODS ========================================O
HRESULT D3DRenderSystem::InvalidateDeviceObjects()
{
	for (int i = 0; i < c_MaxTextureStages; i++)
	{
		SetTexture( 0, i );
	}

	SetCurrentShader( 0 );
	SetRenderTarget	( 0 );

	SAFE_DECREF( backBufferSurf );
	SAFE_DECREF( depthStencilSurf );

	primitiveCache.	InvalidateDeviceObjects();
	textureCache.	InvalidateDeviceObjects();
	shaderCache.	InvalidateDeviceObjects();

	return S_OK;
} // D3DRenderSystem::InvalidateDeviceObjects

/*
void D3DRenderSystem::GetDeviceCaps( DevCaps& caps )
{
	if (pD3D)
	{
		D3DADAPTER_IDENTIFIER8 id;
		pD3D->GetAdapterIdentifier( d3dCaps.AdapterOrdinal, 0, &id );
		strncpy( caps.devDescr,	id.Description, c_MaxDevCapsStr - 1 );
		strncpy( caps.devDriver, id.Driver, c_MaxDevCapsStr - 1 );

	}

	if (d3dCaps.DeviceType == D3DDEVTYPE_HAL) caps.devType = dtHAL;
		else caps.devType = dtREF;

	caps.adapterOrdinal = d3dCaps.AdapterOrdinal;

	if (d3dCaps.DevCaps & D3DDEVCAPS_NPATCHES) 
			caps.hwNPatches = true; else caps.hwNPatches = false;
	if (d3dCaps.DevCaps & D3DDEVCAPS_HWRASTERIZATION  ) 
			caps.hwRasterization = true; else caps.hwRasterization = false;
	if (d3dCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) 
			caps.hwTnL = true; else caps.hwTnL = false;
	if (d3dCaps.DevCaps & D3DDEVCAPS_PUREDEVICE) 
			caps.hwPure  = true; else caps.hwPure = false;
	if (d3dCaps.DevCaps & D3DDEVCAPS_QUINTICRTPATCHES) 
			caps.hwBezier = true; else caps.hwBezier = false;
	if (d3dCaps.DevCaps & D3DDEVCAPS_RTPATCHES ) 
			caps.hwRTPatches = true; else caps.hwRTPatches = false;

	caps.texBlendStages		= d3dCaps.MaxTextureBlendStages;
	caps.texInSinglePass		= d3dCaps.MaxSimultaneousTextures;

	caps.dsfD16			= _CheckDSAvail( D3DFMT_D16,			d3dCaps );
	caps.dsfD15S1		= _CheckDSAvail( D3DFMT_D15S1,			d3dCaps );
	caps.dsfD24X8		= _CheckDSAvail( D3DFMT_D24X8,			d3dCaps );
	caps.dsfD24S8		= _CheckDSAvail( D3DFMT_D24S8,			d3dCaps );
	caps.dsfD24X4S4		= _CheckDSAvail( D3DFMT_D24X4S4,		d3dCaps );
	caps.dsfD32			= _CheckDSAvail( D3DFMT_D32,			d3dCaps );
	caps.dsfD16Lockable	= _CheckDSAvail( D3DFMT_D16_LOCKABLE,	d3dCaps );

	caps.rttARGB4444	= _CheckRenderToTexAvail( D3DFMT_A4R4G4B4,	d3dCaps );
	caps.rttRGB565		= _CheckRenderToTexAvail( D3DFMT_R5G6B5,	d3dCaps );
	caps.rttRGB888		= _CheckRenderToTexAvail( D3DFMT_R8G8B8,	d3dCaps );
	caps.rttA8			= _CheckRenderToTexAvail( D3DFMT_A8,		d3dCaps );
	caps.rttARGB1555	= _CheckRenderToTexAvail( D3DFMT_A1R5G5B5,	d3dCaps );

	/*
	D3DCAPS8 d3dCaps;
    pDevice->GetDeviceCaps( &d3dCaps );

	switch (cap)
	{
	case dcIndexedVertexBlending:
		return (d3dCaps.MaxVertexBlendMatrixIndex >= 255);
	case dcBumpEnvMap:
		return ((d3dCaps.TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP) != 0);
	case dcBumpEnvMapLuminance:
		return ((d3dCaps.TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAPLUMINANCE) != 0);
	default:
		return false;
	}
	*/

	/*
	if (!pD3D) return false;
	D3DDISPLAYMODE mode;
	pD3D->GetAdapterDisplayMode( d3dCaps.AdapterOrdinal, &mode );
	return (pD3D->CheckDeviceFormat( d3dCaps.AdapterOrdinal, 
									 D3DDEVTYPE_HAL, 
									 mode.Format, 
									 D3DUSAGE_DEPTHSTENCIL,
									 D3DRTYPE_SURFACE,
									 fmt
									 ) == D3D_OK);

  if (!pD3D) return false;
	D3DDISPLAYMODE mode;
	pD3D->GetAdapterDisplayMode( d3dCaps.AdapterOrdinal, &mode );
	return (pD3D->CheckDeviceFormat( d3dCaps.AdapterOrdinal, 
									 D3DDEVTYPE_HAL, 
									 mode.Format, 
									 D3DUSAGE_RENDERTARGET,
									 D3DRTYPE_TEXTURE,
									 fmt
									 ) == D3D_OK);
	
} // D3DRenderSystem::_FillDevCaps
*/

const int			c_NumDeviceTypes	= 2;
const char*			c_StrDeviceDescs[]	= {"HAL", "REF" };
const D3DDEVTYPE	c_DeviceTypes[]		= { D3DDEVTYPE_HAL, D3DDEVTYPE_REF };

static int SortModesCallback( const VOID* arg1, const VOID* arg2 )
{
    D3DDISPLAYMODE* p1 = (D3DDISPLAYMODE*)arg1;
    D3DDISPLAYMODE* p2 = (D3DDISPLAYMODE*)arg2;

    if( p1->Format > p2->Format )   return -1;
    if( p1->Format < p2->Format )   return +1;
    if( p1->Width  < p2->Width )    return -1;
    if( p1->Width  > p2->Width )    return +1;
    if( p1->Height < p2->Height )   return -1;
    if( p1->Height > p2->Height )   return +1;

    return 0;
}

/*---------------------------------------------------------------------------*/
/*	Func:	D3DRenderSystem::BuildDeviceList
/*	Desc:	Builds list of available devices and their display modes
/*	Remark:	Adopted from D3D samples 
/*---------------------------------------------------------------------------*/
void D3DRenderSystem::BuildDeviceList()
{
	assert( pD3D );
	int totalAdapters = pD3D->GetAdapterCount();
	nAdapters = 0;
    for (int i = 0; i < totalAdapters; i++)
    {
        // Fill in adapter info
        D3DAdapterInfo& curAdapter = adapterList[i];
        pD3D->GetAdapterIdentifier( i, D3DENUM_NO_WHQL_LEVEL, &(curAdapter.adapterID) );
        pD3D->GetAdapterDisplayMode( i, &(curAdapter.desktopDisplayMode) );
        curAdapter.nDevices	= 0;

        // Enumerate all display modes on this adapter
        D3DDISPLAYMODE	modes[c_MaxDeviceDisplayModes];
        D3DFORMAT		formats[c_MaxDisplayFormatsInMode];
        int				numFormats  = 0;
        int				numModes	= 0;
		int				totalModes	= pD3D->GetAdapterModeCount( i );

        // Add the adapter's current desktop format to the list of formats
        formats[numFormats++] = curAdapter.desktopDisplayMode.Format;

        for (int j = 0; j < totalModes; j++)
        {
            //  getting next display mode
			D3DDISPLAYMODE dispMode;
            pD3D->EnumAdapterModes( i, j, &dispMode );
            
			if (dispMode.Width  < 640 || dispMode.Height < 400) continue;

			//  we select display mode with highest refresh rate available
            bool addMode = true;
			int k = 0;
			for (k = 0; k < numModes; k++)
            {
                if ((modes[k].Width  == dispMode.Width ) &&
                    (modes[k].Height == dispMode.Height) &&
                    (modes[k].Format == dispMode.Format))
				{
					addMode = false;
					if (modes[k].RefreshRate < dispMode.RefreshRate)
					{
						modes[k] = dispMode;
					}
				}
            }

            if (k == numModes && addMode)
            {
                modes[numModes].Width       = dispMode.Width;
                modes[numModes].Height      = dispMode.Height;
                modes[numModes].Format      = dispMode.Format;
                modes[numModes].RefreshRate = dispMode.RefreshRate;
                numModes++;

                //  check if the mode's format already exists
                for (k = 0; k < numFormats; k++)
                {
                    if (dispMode.Format == formats[k]) break;
                }

                if (k == numFormats) formats[numFormats++] = dispMode.Format;
            }
        }
		
        //  sort the list of display modes (by format, then width, then height)
        qsort( modes, numModes, sizeof(D3DDISPLAYMODE), SortModesCallback );

        //  add devices to adapter
        for (int k = 0; k < c_NumDeviceTypes; k++ )
        {
            D3DDeviceInfo& curDevice = curAdapter.devices[curAdapter.nDevices];
            curDevice.devType = c_DeviceTypes[k];
            pD3D->GetDeviceCaps( i, c_DeviceTypes[k], &curDevice.caps );
            curDevice.strDesc       = c_StrDeviceDescs[k];
            curDevice.nModes		= 0;
            curDevice.canDoWindowed = false;

			/*
            // Examine each format supported by the adapter to see if it will
            // work with this device and meets the needs of the application.
            BOOL  bFormatConfirmed[20];
            DWORD dwBehavior[20];
            D3DFORMAT fmtDepthStencil[20];

            for( DWORD f=0; f<dwNumFormats; f++ )
            {
                bFormatConfirmed[f] = FALSE;
                fmtDepthStencil[f] = D3DFMT_UNKNOWN;

                // Skip formats that cannot be used as render targets on this device
                if( FAILED( m_pD3D->CheckDeviceType( iAdapter, pDevice->DeviceType,
                                                     formats[f], formats[f], FALSE ) ) )
                    continue;

                if( pDevice->DeviceType == D3DDEVTYPE_HAL )
                {
                    // This system has a HAL device
                    bHALExists = TRUE;

                    if( pDevice->d3dCaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED )
                    {
                        // HAL can run in a window for some mode
                        bHALIsWindowedCompatible = TRUE;

                        if( f == 0 )
                        {
                            // HAL can run in a window for the current desktop mode
                            bHALIsDesktopCompatible = TRUE;
                        }
                    }
                }

                // Confirm the device/format for HW vertex processing
                if( pDevice->d3dCaps.DevCaps&D3DDEVCAPS_HWTRANSFORMANDLIGHT )
                {
                    if( pDevice->d3dCaps.DevCaps&D3DDEVCAPS_PUREDEVICE )
                    {
                        dwBehavior[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING |
                                        D3DCREATE_PUREDEVICE;

                        if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
                                                      formats[f] ) ) )
                            bFormatConfirmed[f] = TRUE;
                    }

                    if ( FALSE == bFormatConfirmed[f] )
                    {
                        dwBehavior[f] = D3DCREATE_HARDWARE_VERTEXPROCESSING;

                        if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
                                                      formats[f] ) ) )
                            bFormatConfirmed[f] = TRUE;
                    }

                    if ( FALSE == bFormatConfirmed[f] )
                    {
                        dwBehavior[f] = D3DCREATE_MIXED_VERTEXPROCESSING;

                        if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
                                                      formats[f] ) ) )
                            bFormatConfirmed[f] = TRUE;
                    }
                }

                // Confirm the device/format for SW vertex processing
                if( FALSE == bFormatConfirmed[f] )
                {
                    dwBehavior[f] = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

                    if( SUCCEEDED( ConfirmDevice( &pDevice->d3dCaps, dwBehavior[f],
                                                  formats[f] ) ) )
                        bFormatConfirmed[f] = TRUE;
                }

                // Find a suitable depth/stencil buffer format for this device/format
                if( bFormatConfirmed[f] && m_bUseDepthBuffer )
                {
                    if( !FindDepthStencilFormat( iAdapter, pDevice->DeviceType,
                        formats[f], &fmtDepthStencil[f] ) )
                    {
                        bFormatConfirmed[f] = FALSE;
                    }
                }
            }
			*/
            //  add all valid modes to device info
			for (int m = 0; m < numModes; m++)
            {
                for (int f = 0; f < numFormats; f++)
                {
                    if (modes[m].Format == formats[f])
                    {
                        //if( bFormatConfirmed[f] == TRUE )
                        {
                            //  add this mode to the device's list of valid modes
                            D3DModeInfo& curMode = curDevice.modes[curDevice.nModes];
							curMode.width				= modes[m].Width;
                            curMode.height				= modes[m].Height;
                            curMode.format				= modes[m].Format;
							curMode.refreshRate			= modes[m].RefreshRate;
                            /*
							curMode.behavior			= dwBehavior[f];
                            curMode.depthStencil		= fmtDepthStencil[f];
                            */
							curDevice.nModes++;
                        }
                    }
                }
            }

			if (curDevice.nModes > 0) curAdapter.nDevices++;

			/*
            // Select any 640x480 mode for default (but prefer a 16-bit mode)
            for( m=0; m<pDevice->dwNumModes; m++ )
            {
                if( pDevice->modes[m].Width==640 && pDevice->modes[m].Height==480 )
                {
                    pDevice->dwCurrentMode = m;
                    if( pDevice->modes[m].Format == D3DFMT_R5G6B5 ||
                        pDevice->modes[m].Format == D3DFMT_X1R5G5B5 ||
                        pDevice->modes[m].Format == D3DFMT_A1R5G5B5 )
                    {
                        break;
                    }
                }
            }

            // Check if the device is compatible with the desktop display mode
            // (which was added initially as formats[0])
            if( bFormatConfirmed[0] && (pDevice->d3dCaps.Caps2 & D3DCAPS2_CANRENDERWINDOWED) )
            {
                pDevice->bCanDoWindowed = TRUE;
                pDevice->bWindowed      = TRUE;
            }

            // If valid modes were found, keep this device
            if( pDevice->dwNumModes > 0 )
                pAdapter->dwNumDevices++;
        */
		}
        //  if valid devices were found, keep this adapter
        if (curAdapter.nDevices > 0) nAdapters++;
    }
	/*
    // Return an error if no compatible devices were found
    if( 0L == m_dwNumAdapters )
        return D3DAPPERR_NOCOMPATIBLEDEVICES;

    // Pick a default device that can render into a window
    // (This code assumes that the HAL device comes before the REF
    // device in the device array).
    for( DWORD a=0; a<m_dwNumAdapters; a++ )
    {
        for( DWORD d=0; d < m_Adapters[a].dwNumDevices; d++ )
        {
            if( m_Adapters[a].devices[d].bWindowed )
            {
                m_Adapters[a].dwCurrentDevice = d;
                m_dwAdapter = a;
                m_bWindowed = TRUE;

                // Display a warning message
                if( m_Adapters[a].devices[d].DeviceType == D3DDEVTYPE_REF )
                {
                    if( !bHALExists )
                        DisplayErrorMsg( D3DAPPERR_NOHARDWAREDEVICE, MSGWARN_SWITCHEDTOREF );
                    else if( !bHALIsSampleCompatible )
                        DisplayErrorMsg( D3DAPPERR_HALNOTCOMPATIBLE, MSGWARN_SWITCHEDTOREF );
                    else if( !bHALIsWindowedCompatible )
                        DisplayErrorMsg( D3DAPPERR_NOWINDOWEDHAL, MSGWARN_SWITCHEDTOREF );
                    else if( !bHALIsDesktopCompatible )
                        DisplayErrorMsg( D3DAPPERR_NODESKTOPHAL, MSGWARN_SWITCHEDTOREF );
                    else // HAL is desktop compatible, but not sample compatible
                        DisplayErrorMsg( D3DAPPERR_NOHALTHISMODE, MSGWARN_SWITCHEDTOREF );
                }

                return S_OK;
            }
        }
    }

    return D3DAPPERR_NOWINDOWABLEDEVICES;*/
} // D3DRenderSystem::BuildDeviceList

void D3DRenderSystem::DumpDeviceList( FILE* fp )
{
	fprintf( fp, "Dumping available device modes list...\n" );
	fprintf( fp, "Number of adapters: %d\n", nAdapters );
	for (int i = 0; i < nAdapters; i++)
	{
		adapterList[i].Dump( fp );
	}
} // D3DRenderSystem::DumpDeviceList

/*****************************************************************************/
/*	D3DModeInfo implementation
/*****************************************************************************/
void D3DModeInfo::Dump( FILE* fp )
{
	fprintf( fp, "%4d x %4d x %2d", width, height, format == D3DFMT_X8R8G8B8 ? 32 : 16 );
	fprintf( fp, " %3dHz ", refreshRate );

	switch (depthStencil)
	{
	case D3DFMT_D16:
		fprintf( fp, "(D16)\n" );
		break;
	case D3DFMT_D15S1:
	    fprintf( fp, "(D15S1)\n" );
		break;
	case D3DFMT_D24X8:
	    fprintf( fp, "(D24X8)\n" );
		break;
	case D3DFMT_D24S8:
	    fprintf( fp, "(D24S8)\n" );
	    break;
	case D3DFMT_D24X4S4:
	    fprintf( fp, "(D24X4S4)\n" );
	    break;
	case D3DFMT_D32:
	    fprintf( fp, "(D32)\n" );
	    break;
	default:
		fprintf( fp, "\n" );
	}
 
} // D3DModeInfo::Dump

/*****************************************************************************/
/*	D3DDeviceInfo implementation
/*****************************************************************************/
void D3DDeviceInfo::Dump( FILE* fp )
{
	fprintf( fp, "Device: %s NumModes: %d ", 
					strDesc, nModes );
	if (canDoWindowed) fprintf( fp, "WindowMode: Yes\n" );
		else fprintf( fp, "WindowMode: No\n" );
	for (int i = 0; i < nModes; i++)
	{
		modes[i].Dump( fp );
	}
} // D3DModeInfo::Dump

D3DModeInfo* D3DDeviceInfo::FindMode( int width, int height )
{
	for (int i = 0; i < nModes; i++)
	{
		if (modes[i].width == width && modes[i].height == height)
		{
			return &(modes[i]);
		}
	}
	return NULL;
} // D3DDeviceInfo::FindMode

/*****************************************************************************/
/*	D3DAdapterInfo implementation
/*****************************************************************************/
void D3DAdapterInfo::Dump( FILE* fp )
{
	fprintf( fp, "Adapter: %s\nDriver: %s (Ver %d)\nNum Devices: %d\n", 
					adapterID.Description, 
					adapterID.Driver, 
					adapterID.DriverVersion,
					nDevices );
	for (int i = 0; i < nDevices; i++)
	{
		devices[i].Dump( fp );
	}
} // D3DAdapterInfo::Dump

D3DDeviceInfo* D3DAdapterInfo::FindDevice( D3DDEVTYPE devType )
{
	for (int i = 0; i < nDevices; i++)
	{
		if (devices[i].devType == devType) return &(devices[i]);
	}
	return NULL;
} // D3DAdapterInfo::FindDevice

