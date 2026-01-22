/*****************************************************************/
/*  File:   rsTextureDX.cpp
/*  Desc:   Direct3D texture management classes inplementation
/*  Author: Silver, Copyright (C) GSC Game World
/*  Date:   Feb 2002
/*****************************************************************/
#include "stdafx.h"
#include "rsDX.h"

#ifndef _INLINES
#include "rsTextureDX.inl"
#endif // !_INLINES

/*****************************************************************************/
/*	HashedTexture implementation
/*****************************************************************************/
void HashedTexture::Reload( DXDevice* pDevice )
{
	if (descr.getTexUsage() == tuLoadable)
	{
		SAFE_DECREF( iSurface0 );
		SAFE_RELEASE( iTexture );
		iSurface0 = 0;
		CreateFromFile( pDevice, true );
	}
} // HashedTexture::Reload

/*---------------------------------------------------------------------------*/
/*	Func:	HashedTexture::CreateFromFile	
/*	Desc:	
/*---------------------------------------------------------------------------*/
bool HashedTexture::CreateFromFile( DXDevice* pDevice, bool forceFormat )
{
	assert( pDevice );
	InvalidateDeviceObjects( pDevice );

	DXTexture *pTexture = 0;

	if (!forceFormat)
	{
		HRESULT hres = 
		D3DXCreateTextureFromFileEx(	pDevice, 
										name,					//  file name
										D3DX_DEFAULT,			//  width
										D3DX_DEFAULT,			//  height
										D3DX_DEFAULT,			//  number of mip levels
										0,						//  texture usage
										D3DFMT_UNKNOWN,			//  color format
										D3DPOOL_DEFAULT,		//  memory pool
										D3DX_DEFAULT,			//  image filter 
										D3DX_DEFAULT,			//  mip filter
										0,						//  disable color key
										NULL,					//  don't care about image info
										NULL,					//  no palette
										&iTexture				//  put here texture, please
									);
		if (hres != S_OK || !iTexture)
		{
			Log.Warning( "Couldn't load texture <%s>", name.c_str() );
			return false;
		}
	}
	else
	{
		UINT		width  = (descr.getSideX() == -1) ? D3DX_DEFAULT : descr.getSideX();
		UINT		height = (descr.getSideY() == -1) ? D3DX_DEFAULT : descr.getSideY();

		HRESULT hres = 
		D3DXCreateTextureFromFileEx(	
				pDevice, 
				name,										//  file name
				width,										//  width
				height,										//  height
				descr.getNMips(),							//  number of mip levels
				TexUsageG2DX( descr.getTexUsage() ),		//  texture usage
				ColorFormatG2DX( descr.getColFmt() ),		//  color format
				MemoryPoolG2DX( descr.getMemPool() ),		//  memory pool
				D3DX_DEFAULT,								//  image filter 
				D3DX_DEFAULT,								//  mip filter
				0,											//  disable color key
				NULL,										//  don't care about image info
				NULL,										//  no palette
				&iTexture	);
		
		DX_CHK( hres );
		if (hres != S_OK || !iTexture)
		{
			Log.Error( "Couldn't load texture <%s>", name.c_str() );
			return false;
		}
	}

	//  get info into texture description
	D3DSURFACE_DESC sdesc;
	DX_CHK( iTexture->GetLevelDesc( 0, &sdesc ) );
	descr.setSideX		( sdesc.Width  );
	descr.setSideY		( sdesc.Height );
	descr.setColFmt		( ColorFormatDX2G	( sdesc.Format ) );
	descr.setMemPool	( MemoryPoolDX2G	( sdesc.Pool   ) );
	descr.setNMips		( iTexture->GetLevelCount() );
	
	TextureUsage tu = TexUsageDX2G( sdesc.Usage );
	if (tu == tuUnknown) tu = tuLoadable;
	descr.setTexUsage	( tu );
	return true;
} // HashedTexture::CreateFromFile

/*---------------------------------------------------------------------------*/
/*	Func:	HashedTexture::Create	
/*	Desc:	Creates texture resource
/*---------------------------------------------------------------------------*/
bool HashedTexture::Create( DXDevice* pDevice, const TextureDescr* td )
{
	assert( pDevice );
	if (td)
	{
		descr = *td;
	}

	HRESULT result = 
#ifdef _DX9
	pDevice->CreateTexture( descr.getSideX	(), 
							descr.getSideY	(), 
							descr.getNMips	(),
							TexUsageG2DX	( descr.getTexUsage()	),			
							ColorFormatG2DX	( descr.getColFmt()		), 
							MemoryPoolG2DX	( descr.getMemPool()	), 
							&iTexture, NULL
							);
#else
        pDevice->CreateTexture( descr.getSideX	(), 
							descr.getSideY	(), 
							descr.getNMips	(),
							TexUsageG2DX	( descr.getTexUsage()	),			
							ColorFormatG2DX	( descr.getColFmt()		), 
							MemoryPoolG2DX	( descr.getMemPool()	), 
							&iTexture 
							);
#endif
	if (result == D3DERR_OUTOFVIDEOMEMORY)
	{
		Log.Error( "Not enough video memory when creating texture: %s(%dx%d), mp:%d", 
					name.c_str(), descr.getSideX(), descr.getSideY(), descr.getMemPool() );
		return false;
	}

	DX_CHK( result );
	return (result == S_OK);
} // HashedTexture::Create

bool HashedTexture::InvalidateDeviceObjects( DXDevice* pDevice )
{
	if (descr.getClient()) 
	{
		descr.getClient()->Invalidate( &descr ); 
	}
	SAFE_DECREF( iSurface0 );
	SAFE_RELEASE( iTexture );
	iSurface0 = 0;
	return true;
} // HashedTexture::InvalidateDeviceObjects

bool	HashedTexture::RestoreDeviceObjects( DXDevice* pDevice )
{
	assert( iTexture == NULL && iSurface0 == NULL );
	bool res = false;
	if (descr.getTexUsage() == tuLoadable)
	{
		res = CreateFromFile( pDevice, true );
		if (res)
		{
			Log.Info( "Recreating texture <%s> from file - succeed.", name );
		}
		else
		{
			Log.Info( "Recreating texture <%s> from file - FAILED!!!", name );
		}
		return true;
	}
	
	res = Create( pDevice, &descr );
	if (res)
	{
		Log.Info( "Recreating texture <%s> - succeed.", name );
	}
	else
	{
		Log.Info( "Recreating texture <%s> - FAILED!!!", name );
	}
	return true;
} // HashedTexture::RestoreDeviceObjects

int	HashedTexture::GetSizeBytes( DXDevice* pDevice ) const
{
	int res = sizeof( *this );
	if (!iTexture) return res;
	D3DSURFACE_DESC sdesc;
	for (int i = 0; i < descr.getNMips(); i++)
	{
		DX_CHK( iTexture->GetLevelDesc( i, &sdesc ) );	
		res += sdesc.Size;
	}
	return res;
} // HashedTexture::GetSizeBytes

void HashedTexture::DumpStatus( DXDevice* pDevice, FILE* fp )
{
	fprintf( fp, "%5dKb  %s\n", GetSizeBytes( pDevice ) / 1024, name.c_str() );
} // HashedTexture::DumpStatus

/*****************************************************************************/
/*	TextureCache implementation
/*****************************************************************************/
TextureCache::TextureCache()
{
	pDevice		= 0;
} // TextureCache::TextureCache

TextureCache::~TextureCache()
{

} // TextureCache::~TextureCache

void TextureCache::Init( DXDevice* pDev )
{
	pDevice = pDev;
	hash.reset();

	//  NULL texture
	HashedTexture nullTD;
	nullTD.descr.setValues( 0, 0, cfUnknown, mpUnknown, tuUnknown );
	nullTD.name = "NULL";
	int id0 = hash.add( nullTD );
	assert( id0 == 0 );

	resetCurTextures();

} // TextureCache::Init

void TextureCache::Shut()
{
	InvalidateDeviceObjects();
} // TextureCache::Shut

void TextureCache::SaveTexture( int texID, const char* fname )
{
	void ParseExtension( const char* fname, char* ext );
	
	char ext[64];
	ParseExtension( fname, ext );

	D3DXIMAGE_FILEFORMAT type = D3DXIFF_DDS;
	if (!strcmp( ext, "bmp" )) type = D3DXIFF_BMP;
	if (!strcmp( ext, "tga" )) type = D3DXIFF_TGA;
	if (!strcmp( ext, "dds" )) type = D3DXIFF_DDS;
	
	D3DXSaveTextureToFile( fname, type, getDXTex( texID ), 0 );
} // TextureCache::SaveTexture

int	TextureCache::GetUsedTextureMemory()
{
	int res = 0;
	int nTex = hash.numElem();
	for (int i = 1; i < nTex; i++)
	{
		res += hash.elem( i ).GetSizeBytes( pDevice );
	}
	return res;
} // TextureCache::GetUsedTextureMemory

/*---------------------------------------------------------------------------*/
/*	Func:	TextureCache::ReloadTextures	
/*	Desc:	Reloads all loadable textures
/*---------------------------------------------------------------------------*/
void TextureCache::ReloadTextures()
{
	for (int stage = 0; stage < c_MaxTextureStages; stage++)
	{
		SetTexture( 0, stage );
	}

	int nTex = hash.numElem();
	for (int i = 1; i < nTex; i++)
	{
		HashedTexture&	tex		= hash.elem( i );
		TextureDescr&	descr	= tex.descr; 
		if (descr.getTexUsage() == tuLoadable)
		{
			tex.Reload( pDevice );
		}
	}
} // TextureCache::ReloadTextures

bool TextureCache::InvalidateDeviceObjects()
{
	for (int i = 1; i < hash.numElem(); i++)
	{
		hash.elem( i ).InvalidateDeviceObjects( pDevice );
	}
	return true;
} // TextureCache::InvalidateDeviceObjects

bool TextureCache::RestoreDeviceObjects()
{
	for (int i = 1; i < hash.numElem(); i++)
	{
		hash.elem( i ).RestoreDeviceObjects( pDevice );
	}
	return true;
} // TextureCache::RestoreDeviceObjects

int TextureCache::CreateTexture( const char* texName, const TextureDescr& td,
								 TextureClient* client )
{
	HashedTexture htex( texName );
	if (!htex.Create( pDevice, &td ))
	{
		LogStatus();
		return 0;
	}
	int id = hash.add( htex );
	htex.descr.setID( id );
	HashedTexture& newTex = hash.elem( id );
	
	newTex.descr.setClient	( client );
	newTex.descr.setID		( id );
	return id;
} // TextureCache::CreateTexture

/*---------------------------------------------------------------------------*/
/*	Func:	TextureCache::DumpStatus	
/*	Desc:	Dumps texture cache info to the file
/*---------------------------------------------------------------------------*/
void TextureCache::DumpStatus( FILE* fp )
{
	int nTex = hash.numElem();
	fprintf( fp, "\nDumping texture cache...\n" );
	fprintf( fp, "---------------------------------------------------------------------\n" );
	fprintf( fp, "Total textures number: %d\n Texture memory used: ~%d Kb", nTex, 
					GetUsedTextureMemory() / 1024 );
	fprintf( fp, "---------------------------------------------------------------------\n" );
	for (int i = 1; i < nTex; i++)
	{
		fprintf( fp, "%3d:  ", i );
		hash.elem( i ).DumpStatus( pDevice, fp );
	}
	fprintf( fp, "---------------------------------------------------------------------\n" );
} // TextureCache::DumpStatus

void TextureCache::LogStatus()
{
	int nTex = hash.numElem();
	Log.Info	( "Dumping texture cache..." );
	Log.Message	( "---------------------------------------------------------------------" );
	Log.Message	( "Total textures number: %d", nTex );
	Log.Message	( "---------------------------------------------------------------------" );
	int vPool = 0;
	int mPool = 0;
	int sPool = 0;
	int uPool = 0;

	for (int i = 1; i < nTex; i++)
	{
		TextureDescr& td = hash.elem( i ).descr;
		int sz			= hash.elem( i ).GetSizeBytes( pDevice );
		int szkb		= sz / 1024;
		int bytesRem	= sz % 1024;
		Log.Message	( "%3d: size: %8db | pool:%-10s | format: %-10s | usage: %-10s | name: %s", 
						i, sz, 
						td.getPoolStr(), 
						td.getColFmtStr(), 
						td.getUsageStr(),
						hash.elem( i ).name );
		switch ( td.getMemPool() )
		{
		case mpVRAM:	vPool += sz; break;
		case mpSysMem:	sPool += sz; break;
		case mpManaged: mPool += sz; break;
		default:		uPool += sz; 
		}
	}
	Log.Message( "Dump complete. Total allocated texture resources: %db", 
					vPool + sPool + mPool + uPool );
	Log.Message( "Default pool: %db. Managed pool: %db. Sysmem pool: %db.", 
					vPool, mPool, sPool );
	if (uPool > 0)
	{
		Log.Warning( "There are %d Bytes of texture memory allocated with incorrect pool type." );
	}
	Log.Message( "---------------------------------------------------------------------" );
} // TextureCache::LogStatus
	