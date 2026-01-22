/*****************************************************************/
/*  File:   gpSystem.cpp
/*  Desc:   Sprite manager implementation
/*  Author: Silver, Copyright (C) GSC Game World
/*  Date:   Feb 2002
/*****************************************************************/
#include "stdafx.h"
#include "fexPack.h"
#include "rsVertex.h"
#include "kCache.h"

#include "kHash.hpp"
#include "kResource.h"
#include "gpText.h"
#include "gpPackage.h"

#include "gpGN16.h"
#include "gpGU16.h"

#include "gpBitmap.h"
#include "gpSystem.h"
#include "kUtilities.h" 
#include "kStrUtil.h"
#include "kBmptool.h"

#include "direct.h"

#include "gpSeries.h"

#ifndef _INLINES
#include "gpSystem.inl"
#endif // !_INLINES

int				GPSystem::curCacheFactor;

/*****************************************************************/
/*  GPSystem implementation
/*****************************************************************/
int	GPSystem::s_GPTexNum = 64;

GPSystem::GPSystem() 
{
	curCamera			= 0;
	gpScale				= 1.0f;
	curLOD				= 0;
	curDiffuse			= 0xFFFFFFFF;
	curZ				= 0.0f;

	debugFont			= NULL;

	const int c_PackedGPMemBudget	= 10485760;
	const int c_UnpackedGPMemBudget	= 10485760 * 3;

	GPSeq::s_PackSegMgr.SetMemoryBudget( c_PackedGPMemBudget );
	GPSeq::s_PixDataMgr.SetMemoryBudget( c_UnpackedGPMemBudget );

	rootDirectory[0] = 0;
}

GPSystem::~GPSystem()
{
	delete debugFont;
}

void GPSystem::Init( IRenderSystem* irs )
{
	assert( irs );
	RC::iRS = irs;	
	_getcwd( rootDirectory, _MAX_PATH );

	//int nMB = RC::iRS->EstimateTexMemSizeMB();
	//s_GPTexNum = nMB*3/2/8; //  set 75% of tex mem for sprite cache

	GPSeq::s_PixelCache.Init( RC::iRS, this );
	sprBucket.resize( s_GPTexNum );
	for (int i = 0; i < s_GPTexNum; i++)
	{
		sprBucket[i].Init();
		sprBucket[i].setTexture( GPSeq::s_PixelCache.GetItemTextureSurfID( i ) );
	}

	batchMode		= bmDrawImmediately;
	curBucket		= -1;
	curCacheFactor  = GetTickCount();

	static int shPrim	= RC::iRS->GetShaderID( "hudZDisable" );
	static int whiteTex = RC::iRS->GetTextureID( "white.bmp" );

	//  primitive buckets
	const int c_NumAuxVert	= 256;
	lineBucket2D.create			( c_NumAuxVert, 0, vfTnL, ptLineList );
	lineBucket2D.setShader		( shPrim );
	lineBucket2D.setTexture		( whiteTex );

	lineBucket3D.create			( c_NumAuxVert, 0, vf2Tex, ptLineList );
	lineBucket3D.setShader		( shPrim );
	lineBucket3D.setTexture		( whiteTex );

	rectBucket2D.create			( c_NumAuxVert, 0, vfTnL, ptTriangleList );
	rectBucket2D.setShader		( shPrim );
	rectBucket2D.setTexture		( whiteTex );
	rectBucket2D.setIsQuadList	( true );

	rectBucket3D.create			( c_NumAuxVert, 0, vf2Tex, ptTriangleList );
	rectBucket3D.setShader		( shPrim );
	rectBucket3D.setTexture		( whiteTex );
	rectBucket3D.setIsQuadList	( true ); 

	EnableZBuffer( false );

	debugFont = new GPFont();
	debugFont->Init( "Arial", 8 );

#ifndef _NOSTAT
	GPSeriesMgr::Init( this );

	REGISTER_SERIES( Polygons			);
	REGISTER_SERIES( GPCacheMisses		);
	REGISTER_SERIES( TexSwitches		);
	REGISTER_SERIES( Shaders			);
	REGISTER_SERIES( Sprites			);
	REGISTER_SERIES( Dips				);
	REGISTER_SERIES( GPCacheThrashes	);

#endif // _NOSTAT
}  // GPSystem::Init

void GPSystem::DrawFillRect( float x, float y, float w, float h, DWORD color )
{
	if (rectBucket2D.getNVert() >= rectBucket2D.getMaxVert() - 4)
	{
		RC::iRS->Draw( rectBucket2D );
		rectBucket2D.setNInd ( 0 );
		rectBucket2D.setNVert( 0 );
		rectBucket2D.setNPri ( 0 );
	}

	VertexTnL* vert = (VertexTnL*)rectBucket2D.getVertexData() + rectBucket2D.getNVert();
	vert[0].setXYZW( x,		y,		0.0f, 1.0f );
	vert[1].setXYZW( x + w,	y,		0.0f, 1.0f );
	vert[2].setXYZW( x,		y + h,	0.0f, 1.0f );
	vert[3].setXYZW( x + w,	y + h,	0.0f, 1.0f );

	vert[0].setDiffuse( color );
	vert[1].setDiffuse( color );
	vert[2].setDiffuse( color );
	vert[3].setDiffuse( color );
	
	rectBucket2D.incNVert		( 4 );
	rectBucket2D.incNPri		( 2 );
} // GPSystem::DrawFillRect

void GPSystem::DrawLine( float x1, float y1, float x2, float y2, DWORD color )
{	
	if (lineBucket2D.getNVert() >= lineBucket2D.getMaxVert() - 2)
	{
		RC::iRS->Draw( lineBucket2D );
		lineBucket2D.setNInd ( 0 );
		lineBucket2D.setNVert( 0 );
		lineBucket2D.setNPri ( 0 );
	}

	VertexTnL* vert = (VertexTnL*)lineBucket2D.getVertexData() + lineBucket2D.getNVert();
	vert[0].setXYZW( x1, y1, 0.0f, 1.0f );
	vert[0].setDiffuse( color );
	vert[1].setXYZW( x2, y2, 0.0f, 1.0f );
	vert[1].setDiffuse( color );
	lineBucket2D.incNVert		( 2 );
	lineBucket2D.incNPri		( 1 );
} // GPSystem::DrawLine

void GPSystem::DrawRect( float x, float y, float w, float h, DWORD color )
{
	DrawLine( x, y, x + w, y, color );
	DrawLine( x + w, y, x + w, y + h, color );
	DrawLine( x + w, y + h, x, y + h, color );
	DrawLine( x, y + h, x, y, color );
} // GPSystem::DrawRect

void GPSystem::DrawLine( const Vector3D& beg, const Vector3D& end, DWORD color )
{
	if (lineBucket3D.getNVert() >= lineBucket3D.getMaxVert() - 2)
	{
		RC::iRS->Draw( lineBucket3D );
		lineBucket3D.setNInd ( 0 );
		lineBucket3D.setNVert( 0 );
		lineBucket3D.setNPri ( 0 );
	}
	Vertex2t* vert = (Vertex2t*)lineBucket3D.getVertexData() + lineBucket3D.getNVert();
	vert[0].setXYZ( beg.x, beg.y, beg.z );
	vert[0].setDiffuse( color );
	vert[1].setXYZ( end.x, end.y, end.z );
	vert[1].setDiffuse( color );
	lineBucket3D.incNVert		( 2 );
	lineBucket3D.incNPri		( 1 );
} // GPSystem::DrawLine

void GPSystem::DrawString( const char* str, 
							float x, float y, float z,
							DWORD color, float height, Camera* cam )
{
	if (debugFont)
	{
		debugFont->DrawString( str, x, y, z, color, height, cam, taLeftTop );
	}
} // GPSystem::DrawString

void GPSystem::DrawFillRect( const Vector3D& lt, const Vector3D& rt, 
								const Vector3D& lb, const Vector3D& rb, 
								DWORD color )
{
	if (rectBucket3D.getNVert() >= rectBucket3D.getMaxVert() - 4)
	{
		RC::iRS->Draw( rectBucket3D );
		rectBucket3D.setNInd ( 0 );
		rectBucket3D.setNVert( 0 );
		rectBucket3D.setNPri ( 0 );
	}

	Vertex2t* vert = (Vertex2t*)rectBucket3D.getVertexData() + rectBucket3D.getNVert();
	vert[0].setXYZ( lt.x, lt.y,	lt.z );
	vert[1].setXYZ( rt.x, rt.y,	rt.z );
	vert[2].setXYZ( lb.x, lb.y,	lb.z );
	vert[3].setXYZ( rb.x, rb.y,	rb.z );

	vert[0].setDiffuse( color );
	vert[1].setDiffuse( color );
	vert[2].setDiffuse( color );
	vert[3].setDiffuse( color );
	
	rectBucket3D.incNVert		( 4 );
	rectBucket3D.incNPri		( 2 );
} // GPSystem::DrawFillRect

void GPSystem::EnableZBuffer( bool enable )
{
	static bool zEnabled = false;
	static int shSprite2D	= RC::iRS->GetShaderID( "hudZDisable" );
	static int shSprite2DZ	= RC::iRS->GetShaderID( "hud" );

	if (enable == zEnabled) return;
	zEnabled = enable;

	FlushBatches();
	if (enable) GPBucket::s_CurShader = shSprite2DZ;
		else GPBucket::s_CurShader = shSprite2D;
	
} // GPSystem::EnableZBuffer

/*---------------------------------------------------------------------------*/
/*	Func:	GPSystem::GetGPAlpha	
/*	Desc:	returns alpha value of the given pixel inside sprite
/*	Parm:	ptX, ptY - points coordinates in local sprite space
/*			precise - when true, alpha is returned even if there is no pixel 
/*				data in system memory
/*	Ret:	Alpha value or, if pixel data is unavailable:
/*				c_InsideChunks - pixel is inside chunks
/*				c_OutsideChunks - pixel is outside chunks
/*---------------------------------------------------------------------------*/
DWORD GPSystem::GetGPAlpha( int seqID, int frameIdx, 
							int ptX, int ptY, 
							bool precise )
{
	if (seqID < 0 || seqID > GetGPNum()) return c_OutsideChunks;

	BYTE* pChunk = 0;
	GPSeq* pSeq = GetSeq( seqID );
	assert( pSeq );
	GPFrame* pFrame = pSeq->GetFrame( frameIdx );
	assert( pFrame );
	
	if (ptX > pFrame->width || ptY > pFrame->height) 
	{
		return c_OutsideChunks;
	}

	GPFrameInst* pFrameInst = pFrame->lastFrameInst;
	if (!pFrameInst)
	{
		pFrameInst = GetFrameInst( seqID, frameIdx, 0 );
	}

	BYTE* pixData = pFrameInst->GetPixelData();
	
	for (int i = 0; i < pFrame->nChunks; i++)
	{
		GPChunkHeader*	chunk = (GPChunkHeader*) pixData;
		int side = chunk->GetSide();
		
		//  check if point is inside chunk square
		if (ptX < chunk->GetX() || 
			ptY < chunk->GetY() ||
			ptX > chunk->GetX() + side || 
			ptY > chunk->GetY() + side) 
		{
			pixData += chunk->GetSizeBytes(); 
			continue;
		}

		if (!precise) return c_InsideChunks;
		//  precise per-pixel testing
		WORD* pPix = (WORD*) chunk->GetPixelData();
		int idx = ptX - chunk->GetX() + (ptY - chunk->GetY()) * side;
		return (pPix[idx] & 0xF000) >> 12;
	}
	return c_OutsideChunks;
} // GPSystem::GetGPAlpha

/*---------------------------------------------------------------*
/*  Func:	GPSystem::PreLoadGPImage
/*	Desc:	Preloads gp information, actual data is not loaded yet
/*	Parm:	gpName - file name of sprite package, without extension
/*	Ret:	identifier of the sprite package
/*---------------------------------------------------------------*/
int	GPSystem::PreLoadGPImage( const char* gpName )
{
	int seqID = gpHash.find( gpName );
	if (seqID == NO_ELEMENT)
	{
		//  right now we push NULL pointer to the GPSeq,
		//  later, in LoadGP it will be created
		seqID = gpHash.add( gpName, NULL ); 
	}

	assert( seqID != NO_ELEMENT );
	return seqID;
}//  PreLoadGPImage

bool GPSystem::LoadGP( int seqID )
{
	GPSeq *pGps = gpHash.elem( seqID );
	if (!pGps)
	{
		if (rootDirectory[0])
		{
			char fullPath[_MAX_PATH];
			strcpy( fullPath, rootDirectory );
			strcat_s( fullPath,sizeof(fullPath), "\\" );
			strcat_s( fullPath,sizeof(fullPath), gpHash.entry( seqID ).key );
			pGps = GPSeq::CreateHeaderFromFile( fullPath );
		}
		else
		{
			pGps = GPSeq::CreateHeaderFromFile( gpHash.entry( seqID ).key );	
		}

		if (!pGps) return false;	
		pGps->SetID( seqID );
		gpHash.entry( seqID ).el = pGps;
	}
	return true;
}

void GPSystem::UnLoadGP( int seqID )
{
	todo( "GPSystem::UnLoadGP" );
}

void GPSystem::CleanBuckets()
{
	for (int i = 0; i < s_GPTexNum; i++)
	{
		if (sprBucket[i].getNVert() == 0) continue;
		
		sprBucket[i].setNVert( 0 );
	}
} // GPSystem::CleanBuckets

void GPSystem::DrawBuckets()
{
	for (int i = 0; i < s_GPTexNum; i++)
	{
		sprBucket[i].Flush();
	}
	FlushPrimBucket();
} // GPSystem::drawBuckets

const float c_BZMin	= 0.4f;
const float c_BZMax	= 0.9f;

void GPSystem::dbgDrawBucketZs( int bucketIdx )
{
	int nV = sprBucket[bucketIdx].getNVert();
	VertexTnL* vert = (VertexTnL*)sprBucket[bucketIdx].getVertexData();
	for (int i = 0; i < nV; i += 4)
	{
		if (vert->z == 0.0f) continue;
		float h = (vert->z - c_BZMin) * 32.0f / (c_BZMax - c_BZMin);
		if (h < 0) h = 0;
		BYTE col = ( i * 10 ) % 255;
		DrawLine( vert->x, vert->y, vert->x, vert->y - h, 0xFF000000 | col );
		vert++;
	}
}

void GPSystem::OnFrame()
{
	GPSeq::s_PixelCache.OnFrame();
	FlushBatches();

#ifndef _NOSTAT
	GPSeriesMgr::OnFrame();
#endif // _NOSTAT

	if (GetKeyState( VK_HOME ) < 0) 
	{
		GPSeq::s_PixelCache.Dump();
		RC::iRS->Dump();
	}
} // GPSystem::OnFrame

void GPSystem::FlushBatches()
{
	DrawBuckets();
	FlushPrimBucket();
	curCacheFactor = GetTickCount();
	GPFrame::curCacheFactor = curCacheFactor;
	BaseResource::curCacheFactor = curCacheFactor;
} // GPSystem::FlushBatches

/*---------------------------------------------------------------------------*/
/*	Func:	GPSystem::DrawSprite
/*	Desc:	Generic sprite drawing function
/*	Parm:	gpID		- package ID
/*			sprID		- sprite ID
/*			x, y, z		- position in screen space
/*---------------------------------------------------------------------------*/
bool GPSystem::DrawSprite(	int	gpID, int sprID, 
							float x, float y, float z,
							bool mirrored, DWORD color )
{
	if (gpID < 0 ||
		sprBucket.size() == 0 // hack to prevent drawing with uninited gpsystem
		) return false;
	GPFrameInst* frameInst = GetFrameInstForDrawing( gpID, sprID, color, 0/*curLOD*/ );
	if (frameInst == NULL) return false;

	//  move chunks to the batch 
	int	nChunks	= frameInst->GetNChunks(); 
	int	numV	= nChunks * 4;

	if (gpScale == 1.0f)
	{
		for (int i = 0; i < nChunks; i++)
		{
			GPChunk&	chunk	= frameInst->GetChunk( i );
			GPBucket&	bucket	= sprBucket[chunk.GetSurfID()];
			bucket.SetCurrentVertexFormat( vfTnL );
			if (mirrored)
			{
				bucket.AddQuadMirrored( chunk, x, y, z, curDiffuse );
			}
			else
			{
				bucket.AddQuad( chunk, x, y, z, curDiffuse );
			}
		}
	}
	else
	{
		for (int i = 0; i < nChunks; i++)
		{
			GPChunk&	chunk	= frameInst->GetChunk( i );
			GPBucket&	bucket	= sprBucket[chunk.GetSurfID()];
			bucket.SetCurrentVertexFormat( vfTnL );
			if (mirrored)
			{
				bucket.AddQuadMirroredScaled( chunk, x, y, z, gpScale, curDiffuse );
			}
			else
			{
				bucket.AddQuadScaled( chunk, x, y, z, gpScale, curDiffuse );
			}
		}
	}
	return true;
} // GPSystem::DrawSprite

/*---------------------------------------------------------------------------*/
/*	Func:	GPSystem::DrawSpriteTransformed
/*	Desc:	Draws post-transformed in screen space sprite
/*	Parm:	gpID		- package ID
/*			sprID		- sprite ID
/*			transf		- screen-space transform matrix 
/*---------------------------------------------------------------------------*/
bool GPSystem::DrawSpriteTransformed( int gpID, int sprID, const Matrix4D& transf, DWORD color )
{
	if (gpID < 0) return false;
	GPFrameInst* frameInst = GetFrameInstForDrawing( gpID, sprID, color, curLOD );
	if (frameInst == NULL) return false;

	//  move chunks to the batch 
	int	nChunks	= frameInst->GetNChunks(); 
	int	numV	= nChunks * 4;

	for (int i = 0; i < nChunks; i++)
	{
		GPChunk&	chunk	= frameInst->GetChunk( i );
		GPBucket&	bucket	= sprBucket[chunk.GetSurfID()];
		bucket.SetCurrentVertexFormat( vfTnL );
		bucket.AddQuad( chunk, transf, curDiffuse );
	}
	return true;
} // GPSystem::DrawSpriteTransformed

bool GPSystem::DrawSpriteAligned(	int gpID, int sprID, 
									const Plane& pl, 
									float x, float y, float	z,
									bool mirrored,
									DWORD color )
{
	if (gpID < 0) return false;
	GPFrameInst* frameInst = GetFrameInstForDrawing( gpID, sprID, color, curLOD );
	if (frameInst == NULL) return false;

	//  move chunks to the batch 
	int	nChunks	= frameInst->GetNChunks(); 
	int	numV	= nChunks * 4;

	
	for (int i = 0; i < nChunks; i++)
	{
		GPChunk&	chunk	= frameInst->GetChunk( i );
		GPBucket&	bucket	= sprBucket[chunk.GetSurfID()];
		bucket.SetCurrentVertexFormat( vfTnL );
		if (mirrored)
		{
			bucket.AddQuadMirroredAligned( chunk, pl, x, y, z, gpScale, curDiffuse );
		}
		else
		{
			bucket.AddQuadAligned( chunk, pl, x, y, z, gpScale, curDiffuse );
		}
	}
	return true;
} // GPSystem::DrawSpriteAligned

/*---------------------------------------------------------------------------*/
/*	Func:	GPSystem::DrawSpriteVS	
/*	Desc:	Draw sprite mimicking software vertex shader
/*---------------------------------------------------------------------------*/
bool GPSystem::DrawSpriteVS( int gpID, int sprID, DWORD color, const SoftwareVS& vs )
{
	if (gpID < 0) return false;
	GPFrameInst* frameInst = GetFrameInstForDrawing( gpID, sprID, color, curLOD );
	if (frameInst == NULL) return false;

	//  move chunks to the batch 
	int	nChunks	= frameInst->GetNChunks(); 
	int	numV	= nChunks * 4;

	for (int i = 0; i < nChunks; i++)
	{
		GPChunk&	chunk	= frameInst->GetChunk( i );
		GPBucket&	bucket	= sprBucket[chunk.GetSurfID()];
		bucket.SetCurrentVertexFormat( vf2Tex );
		bucket.AddQuadVS( chunk, vs, curDiffuse );
	}

	return true;
} // GPSystem::DrawSpriteVS

/*---------------------------------------------------------------------------*/
/*	Func:	GPSystem::DrawSprite
/*	Desc:	Generic sprite drawing function
/*	Parm:	gpID		- package ID
/*			sprID		- sprite ID
/*			x, y, z		- position in screen space
/*---------------------------------------------------------------------------*/
bool GPSystem::DrawWSprite(	int	gpID, int sprID, 
							float x, float y, float z,
							bool mirrored, DWORD color )
{
	if (gpID < 0) return false;
	GPFrameInst* frameInst = GetFrameInstForDrawing( gpID, sprID, color, curLOD );
	if (frameInst == NULL) return false;

	//  move chunks to the batch 
	int	nChunks	= frameInst->GetNChunks(); 
	int	numV	= nChunks * 4;

	if (gpScale == 1.0f)
	{
		for (int i = 0; i < nChunks; i++)
		{
			GPChunk&	chunk	= frameInst->GetChunk( i );
			GPBucket&	bucket	= sprBucket[chunk.GetSurfID()];
			bucket.SetCurrentVertexFormat( vf2Tex );
			if (mirrored)
			{
				bucket.AddQuadMirrored( chunk, x, y, z, curDiffuse );
			}
			else
			{
				bucket.AddQuad( chunk, x, y, z, curDiffuse );
			}
		}
	}
	else
	{
		for (int i = 0; i < nChunks; i++)
		{
			GPChunk&	chunk	= frameInst->GetChunk( i );
			GPBucket&	bucket	= sprBucket[chunk.GetSurfID()];
			bucket.SetCurrentVertexFormat( vf2Tex );
			if (mirrored)
			{
				bucket.AddQuadMirroredScaled( chunk, x, y, z, gpScale, curDiffuse );
			}
			else
			{
				bucket.AddQuadScaled( chunk, x, y, z, gpScale, curDiffuse );
			}
		}
	}
	return true;
} // GPSystem::DrawWSprite

/*---------------------------------------------------------------------------*/
/*	Func:	GPSystem::DrawWSpriteTransformed
/*	Desc:	Draws post-transformed in screen space sprite
/*	Parm:	gpID		- package ID
/*			sprID		- sprite ID
/*			transf		- screen-space transform matrix 
/*---------------------------------------------------------------------------*/
bool GPSystem::DrawWSpriteTransformed( int gpID, int sprID, const Matrix4D& transf, DWORD color )
{
	if (gpID < 0) return false;
	GPFrameInst* frameInst = GetFrameInstForDrawing( gpID, sprID, color, curLOD );
	if (frameInst == NULL) return false;

	//  move chunks to the batch 
	int	nChunks	= frameInst->GetNChunks(); 
	int	numV	= nChunks * 4;

	for (int i = 0; i < nChunks; i++)
	{
		GPChunk&	chunk	= frameInst->GetChunk( i );
		GPBucket&	bucket	= sprBucket[chunk.GetSurfID()];
		bucket.SetCurrentVertexFormat( vf2Tex );
		bucket.AddQuad( chunk, transf, curDiffuse );
	}
	return true;
} // GPSystem::DrawWSpriteTransformed

bool GPSystem::DrawWSpriteAligned(	int gpID, int sprID, 
									const Plane& pl, 
									float x, float y, float	z,
									bool mirrored,
									DWORD color )
{
	if (gpID < 0) return false;
	GPFrameInst* frameInst = GetFrameInstForDrawing( gpID, sprID, color, curLOD );
	if (frameInst == NULL) return false;

	//  move chunks to the batch 
	int	nChunks	= frameInst->GetNChunks(); 
	int	numV	= nChunks * 4;

	
	for (int i = 0; i < nChunks; i++)
	{
		GPChunk&	chunk	= frameInst->GetChunk( i );
		GPBucket&	bucket	= sprBucket[chunk.GetSurfID()];
		bucket.SetCurrentVertexFormat( vf2Tex );
		bucket.AddQuadAligned( chunk, pl, x, y, z, gpScale, curDiffuse );
	}
	return true;
} // GPSystem::DrawWSpriteAligned

/*****************************************************************
/*	TexItemLayout implementation
/*****************************************************************/
TexItemLayout::TexItemLayout()
{
	maxLevel = c_MaxQuadLevels;
	texID = 0;
}

void TexItemLayout::Init( int texSide )
{
	maxPixelCapacity = texSide * texSide;
	maxLevel = 0;
	while (texSide > 1) 
	{
		texSide >>= 1;
		maxLevel++;				
	}
	Free();
}

/*****************************************************************
/*	QuadLayoutLevel implementation
/*****************************************************************/
void QuadLayoutLevel::Dump()
{
	Log.Message( "Free chunks: %d. ", numQuads );
	for (int i = 0; i < numQuads; i++)
	{
		Log.Message( "%d %d", quad[i].x, quad[i].y );
	}
}










