/*****************************************************************************/
/*	File:	rsPrimitiveBufferDX.cpp
/*	Desc:	
/*	Author:	Ruslan Shestopalyuk
/*	Date:	26.12.2002
/*****************************************************************************/
#include "stdafx.h"

#include "rsVertex.h"
#include "rsDX.h"
#include "kUtilities.h"

#ifndef _INLINES
#include "rsPrimitiveBufferDX.inl"
#endif // _INLINES

/*****************************************************************************/
/*	PrimitiveCache implementation
/*****************************************************************************/
DXDevice*			IndexBuffer::dev	= 0;
DXDevice*			VertexBuffer::dev	= 0;
VertexBuffer*		VertexBuffer::curVB = 0;


IndexBuffer*		IndexBuffer::curIB	= 0;

void PrimitiveCache::Init( DXDevice* pDev )
{
	pDevice = pDev;
	assert( pDevice );

	dynamicIB.Create( pDevice, c_DynamicBufferInd	);
	staticIB.Create	( pDevice, c_StaticBufferInd	);
	quadIB.Create	( pDevice, c_QuadBufferInd		);

	staticVB.Create	( pDevice, c_StaticVBufferBytes, vfUnknown );
	dynamicVB.Create( pDevice, c_DynamicVBufferBytes, vfUnknown );

	for (int i = 1; i < c_NumVertexTypes; i++)
	{
		pbQuad[i].AttachVB		( &dynamicVB	);
		pbQuad[i].AttachIB		( &quadIB		);
		pbQuad[i].SetVertexFormat( (VertexFormat)i );

		pbStatic[i].AttachVB	( &staticVB		);
		pbStatic[i].AttachIB	( &staticIB		);
		pbStatic[i].SetVertexFormat( (VertexFormat)i );

		pbDynamic[i].AttachVB	( &dynamicVB	);
		pbDynamic[i].AttachIB	( &dynamicIB	);
		pbDynamic[i].SetVertexFormat( (VertexFormat)i );
	}

	pbVshStatic.AttachVB		( &staticVB	);
	pbVshStatic.AttachIB		( &staticIB	);

	pbVshDynamic.AttachVB		( &dynamicVB );
	pbVshDynamic.AttachIB		( &dynamicIB );
} // PrimitiveCache::Init

void PrimitiveCache::Shut()
{
	quadIB.Release();
	staticIB.Release();
	dynamicIB.Release();

	staticVB.Release();
	dynamicVB.Release();
} // PrimitiveCache::Shut

bool PrimitiveCache::RestoreDeviceObjects()
{
	Init( pDevice );
	return true;
} // PrimitiveCache::InvalidateDeviceObjects

bool PrimitiveCache::InvalidateDeviceObjects()
{
	Shut();
	return true;
} // PrimitiveCache::InvalidateDeviceObjects

/*****************************************************************************/
/*	PrimitiveBuffer implementation
/*****************************************************************************/
DWORD PrimitiveBuffer::curFVF = 0;

/*****************************************************************************/
/*	VertexBuffer implementation
/*****************************************************************************/
void VertexBuffer::Release()
{
	int nRef = 0;
	SAFE_RELEASE( dxVB );
}

void VertexBuffer::Create( DXDevice* pDevice, int nBytes, VertexFormat vf )
{
	curStride	= 0;
	dev			= pDevice;
	curVert		= 0;
	curByte		= 0;
	maxByte		= nBytes;
	curFVF		= 0;
} // VertexBuffer::Create

/*****************************************************************************/
/*	IndexBuffer implementation
/*****************************************************************************/
void IndexBuffer::Release()
{
	SAFE_RELEASE( dxIB );
} // IndexBuffer::Release

bool IndexBuffer::Create( DXDevice* pDevice, int nInd )
{
	dev = pDevice;
	maxIdx = nInd;
	maxByte = nInd * c_IBStride;
	curIdx = 0;
	assert( dev );
	return true;
} // IndexBuffer::Create

/*****************************************************************************/
/*	StaticIB implementation
/*****************************************************************************/
bool StaticIB::Create( DXDevice* pDevice, int nInd )
{
	IndexBuffer::Create( pDevice, nInd );
	int sizeBytes = nInd * c_IBStride;
#ifdef _DX9
	DX_CHK( pDevice->CreateIndexBuffer(	sizeBytes, 
										D3DUSAGE_WRITEONLY, 
										D3DFMT_INDEX16, 
										D3DPOOL_DEFAULT, 
										&dxIB,
                                        NULL     
										) );
#else
    DX_CHK( pDevice->CreateIndexBuffer(	sizeBytes, 
										D3DUSAGE_WRITEONLY, 
										D3DFMT_INDEX16, 
										D3DPOOL_DEFAULT, 
										&dxIB
										) );
#endif
	softwareVP = false;
	return true;
} // StaticIB::Create

bool StaticIB::FillData( WORD* idxPtr, int nInd, BaseMesh& bm )
{
	if (bm.getIBPos() != -1) return true;
	int nLockBytes = nInd * c_IBStride;
	
	assert( dxIB );
	if (nLockBytes > maxByte || curByte + nLockBytes > maxByte) 
	{ 
		assert( false ); 
		return false; 
	}
	
	DWORD flags = D3DLOCK_NOSYSLOCK;	

	BYTE* data = 0;
	DX_CHK( dxIB->Lock( curByte, nLockBytes, &data, flags ) );
	if (data) util::memcpy( data, idxPtr, nLockBytes );
	DX_CHK( dxIB->Unlock() );
	
	bm.setIBPos( curIdx );
	curIdx += nInd;
	curByte += nLockBytes;
	return true;
}

/*****************************************************************************/
/*	DynamicIB implementation
/*****************************************************************************/
bool DynamicIB::Create( DXDevice* pDevice, int nInd )
{
	IndexBuffer::Create( pDevice, nInd );
	
	int sizeBytes = nInd * c_IBStride;
#ifdef _DX9
    DX_CHK( pDevice->CreateIndexBuffer(	sizeBytes, 
										D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
										D3DFMT_INDEX16, 
										D3DPOOL_DEFAULT, 
										&dxIB,
                                        NULL
										) );
#else
    DX_CHK( pDevice->CreateIndexBuffer(	sizeBytes, 
										D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
										D3DFMT_INDEX16, 
										D3DPOOL_DEFAULT, 
										&dxIB
										) );
#endif 

	
	softwareVP = false;
	return true;
} // DynamicIB::Create

bool DynamicIB::FillData( WORD* idxPtr, int nInd, BaseMesh& bm )
{
	int nLockBytes = nInd * c_IBStride;
	
	assert( dxIB );
	if (nLockBytes > maxByte) 
	{ 
		assert( false ); 
		return false; 
	}
	
	DWORD flags = D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE;	
	if (curByte + nLockBytes > maxByte)
	{
		curByte = 0;
		curIdx = 0;
		flags = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
	}

	BYTE* data = 0;
	DX_CHK( dxIB->Lock( curByte, nLockBytes, &data, flags ) );
	if (data) util::memcpy( data, idxPtr, nLockBytes );
	DX_CHK( dxIB->Unlock() );
	
	bm.setIBPos( curIdx );
	curIdx += nInd;
	curByte += nLockBytes;
	return true;
} // DynamicIB::FillData

/*****************************************************************************/
/*	SoftwareProcessIB implementation
/*****************************************************************************/
bool SoftwareProcessIB::Create( DXDevice* pDevice, int nInd )
{
	IndexBuffer::Create( pDevice, nInd );
	
	int sizeBytes = nInd * c_IBStride;
#ifdef _DX9
	DX_CHK( pDevice->CreateIndexBuffer(	sizeBytes, 
										D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC | D3DUSAGE_SOFTWAREPROCESSING, 
										D3DFMT_INDEX16, 
										D3DPOOL_DEFAULT, 
										&dxIB,
                                        NULL
										) );
#else
    DX_CHK( pDevice->CreateIndexBuffer(	sizeBytes, 
										D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC | D3DUSAGE_SOFTWAREPROCESSING, 
										D3DFMT_INDEX16, 
										D3DPOOL_DEFAULT, 
										&dxIB
										) );
#endif
	softwareVP = false;
	return true;
} // SoftwareProcessIB::Create

bool SoftwareProcessIB::FillData( WORD* idxPtr, int nInd, BaseMesh& bm )
{
	int nLockBytes = nInd * c_IBStride;
	
	assert( dxIB );
	if (nLockBytes > maxByte) 
	{ 
		assert( false ); 
		return false; 
	}
	
	DWORD flags = D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE;	
	if (curByte + nLockBytes > maxByte)
	{
		curByte = 0;
		curIdx = 0;
		flags = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
	}

	BYTE* data = 0;
	DX_CHK( dxIB->Lock( curByte, nLockBytes, &data, flags ) );
	if (data) util::memcpy( data, idxPtr, nLockBytes );
	DX_CHK( dxIB->Unlock() );
	
	bm.setIBPos( curIdx );
	curIdx += nInd;
	curByte += nLockBytes;
	return true;
} // SoftwareProcessIB::FillData

/*****************************************************************************/
/*	QuadIB implementation
/*****************************************************************************/
bool QuadIB::Create( DXDevice* pDevice, int nInd )
{
	StaticIB::Create( pDevice, nInd );

	BYTE* data = 0;
	DX_CHK( dxIB->Lock( 0, nInd * c_IBStride, &data, D3DLOCK_NOSYSLOCK ) );
	WORD* idxData = (WORD*) data;
	assert( idxData );
	//  fill with quad indices
	int nQuads = nInd / 6;
	int cV = 0;
	int cI = 0;
	for (int i = 0; i < nQuads; i++)
	{
		idxData[cI++] = cV;
		idxData[cI++] = cV + 1;
		idxData[cI++] = cV + 2;
		idxData[cI++] = cV + 2;
		idxData[cI++] = cV + 1;
		idxData[cI++] = cV + 3;
		cV += 4;
	}
	
	maxIdx = nInd;
	curIdx = nInd;
	
	DX_CHK( dxIB->Unlock() );
	return true;
} // QuadIB::Create

/*****************************************************************************/
/*	SharedIB implementation
/*****************************************************************************/
bool SharedIB::FillData( WORD* idxPtr, int nInd, BaseMesh& bm )
{
	//  nothing is done here, because index data is already there
	bm.setIBPos( 0 );
	return true;
}

/*****************************************************************************/
/*	StaticVB implementation
/*****************************************************************************/
void StaticVB::Create( DXDevice* pDevice, int nBytes, VertexFormat vf )
{
	VertexBuffer::Create( pDevice, nBytes, vf );
#ifdef _DX9
	DX_CHK( pDevice->CreateVertexBuffer(	nBytes, 
											D3DUSAGE_WRITEONLY,
											0, 
											D3DPOOL_DEFAULT, 
											&dxVB,
                                            NULL
										) );
#else
    DX_CHK( pDevice->CreateVertexBuffer(	nBytes, 
											D3DUSAGE_WRITEONLY,
											0, 
											D3DPOOL_DEFAULT, 
                                            &dxVB
										) );
#endif
	softwareVP = false;
} // StaticVB::Create

void StaticVB::FillData( void* vert, int nVert, BaseMesh& bm, int newStride )
{
	if (bm.getVBPos() != -1) return;
	assert( newStride != 0 );

	int nLockBytes = nVert * newStride;
	assert( dxVB );
	if (nLockBytes > maxByte || curByte + nLockBytes > maxByte) 
	{ 
		assert( false ); 
		return; 
	}
	
	DWORD flags = D3DLOCK_NOSYSLOCK;	

	int remainder = curByte % newStride;
	if (remainder > 0) curByte += newStride - remainder;
	curVert = curByte / newStride;

	BYTE* data = 0;
	DX_CHK( dxVB->Lock( curByte, nLockBytes, &data, flags ) );
	if (data) util::memcpy( data, vert, nLockBytes );
	DX_CHK( dxVB->Unlock() );
	
	bm.setVBPos( curVert );
	curByte += nLockBytes;
	curVert += nVert;
} // StaticVB::FillData

/*****************************************************************************/
/*	DynamicVB implementation
/*****************************************************************************/
void DynamicVB::Create( DXDevice* pDevice, int nBytes, VertexFormat vf )
{
	VertexBuffer::Create( pDevice, nBytes, vf );
#ifdef _DX9
	DX_CHK( pDevice->CreateVertexBuffer(	nBytes, 
											D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
											0, 
											D3DPOOL_DEFAULT, 
											&dxVB,
                                            NULL                
										) );
#else
    DX_CHK( pDevice->CreateVertexBuffer(	nBytes, 
											D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC,
											0, 
											D3DPOOL_DEFAULT, 
											&dxVB
										) );
#endif
	softwareVP = false;
} // DynamicVB::Create

void DynamicVB::FillData( void* vert, int nVert, BaseMesh& bm, int newStride )
{
	assert( newStride != 0 );
	int nLockBytes = nVert * newStride;
	
	assert( dxVB );
	if (nLockBytes > maxByte) 
	{ 
		assert( false ); 
		return; 
	}
	
	DWORD flags = D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE;	
	if (curByte + nLockBytes > maxByte - 64)
	{
		curByte = 0;
		curVert = 0;
		flags = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
	}
	else
	//  align curByte to the new stride
	{
		int remainder = curByte % newStride;
		if (remainder > 0) curByte += newStride - remainder;
		curVert = curByte / newStride;
	}

	BYTE* data = 0;
	DX_CHK( dxVB->Lock( curByte, nLockBytes, &data, flags ) );
	if (data) util::memcpy( data, vert, nLockBytes );
	DX_CHK( dxVB->Unlock() );
	
	bm.setVBPos( curVert );
	curByte += nLockBytes;
	curVert += nVert;
} // DynamicVB::FillData

/*****************************************************************************/
/*	SoftwareProcessVB implementation
/*****************************************************************************/
void SoftwareProcessVB::Create( DXDevice* pDevice, int nBytes, VertexFormat vf )
{
	VertexBuffer::Create( pDevice, nBytes, vf );
#ifdef _DX9
	DX_CHK( pDevice->CreateVertexBuffer(	nBytes, 
											D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC | D3DUSAGE_SOFTWAREPROCESSING,
											0, 
											D3DPOOL_DEFAULT, 
                                            &dxVB,
                                            NULL
										) );
#else
    DX_CHK( pDevice->CreateVertexBuffer(	nBytes, 
											D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC | D3DUSAGE_SOFTWAREPROCESSING,
											0, 
											D3DPOOL_DEFAULT, 
											&dxVB
										) );
#endif
	softwareVP = true;
} // SoftwareProcessVB::Create

void SoftwareProcessVB::FillData( void* vert, int nVert, BaseMesh& bm, int newStride )
{
	assert( newStride != 0 );
	int nLockBytes = nVert * newStride;
	
	assert( dxVB );
	if (nLockBytes > maxByte) 
	{ 
		assert( false ); 
		return; 
	}
	
	DWORD flags = D3DLOCK_NOSYSLOCK | D3DLOCK_NOOVERWRITE;	
	if (curByte + nLockBytes > maxByte)
	{
		curByte = 0;
		curVert = 0;
		flags = D3DLOCK_NOSYSLOCK | D3DLOCK_DISCARD;
	}
	else
	//  align curByte to the new stride
	{
		int remainder = curByte % newStride;
		if (remainder > 0) curByte += newStride - remainder;
		curVert = curByte / newStride;
	}

	BYTE* data = 0;
	DX_CHK( dxVB->Lock( curByte, nLockBytes, &data, flags ) );
	if (data) util::memcpy( data, vert, nLockBytes );
	DX_CHK( dxVB->Unlock() );
	
	bm.setVBPos( curVert );
	curByte += nLockBytes;
	curVert += nVert;
} // SoftwareProcessVB::FillData
