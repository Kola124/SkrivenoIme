/*****************************************************************************/
/*	File:	rsPrimitiveBufferDX.inl
/*	Author:	Ruslan Shestopalyuk
/*	Date:	05.02.2003
/*****************************************************************************/

const DWORD  c_FVF[] = 
{
	0, // vfUnknown		
	D3DFVF_XYZRHW	| D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1, // vfTnL			
	D3DFVF_XYZ		| D3DFVF_DIFFUSE | D3DFVF_TEX2, // vf2Tex			
	D3DFVF_XYZ		| D3DFVF_NORMAL	 | D3DFVF_TEX1, // vfN				
	D3DFVF_XYZRHW	| D3DFVF_DIFFUSE | D3DFVF_TEX2, // vfTnL2			
	D3DFVF_XYZ		| D3DFVF_TEX1, // vfT				
	D3DFVF_XYZB1	| D3DFVF_LASTBETA_UBYTE4 | D3DFVF_TEX2, // vfMP1			
	D3DFVF_XYZB1 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX2, // vfNMP1			
	D3DFVF_XYZRHW	| D3DFVF_DIFFUSE |D3DFVF_SPECULAR | D3DFVF_TEX2, // vfTnL2S			
	D3DFVF_XYZB2 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX2, // vfNMP2			
	D3DFVF_XYZB3 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX2, // vfNMP3			
	D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX2 // vfNMP4			
};

_inl DWORD VertexFormatFVF( VertexFormat vf )
{
	assert( vf < c_NumVertexTypes );
	return c_FVF[(int)vf];
}

_inl D3DPRIMITIVETYPE PriTypeDX( PrimitiveType priType )
{
	D3DPRIMITIVETYPE d3dPri = D3DPT_POINTLIST;
	switch (priType)
	{
		case ptTriangleList:	d3dPri = D3DPT_TRIANGLELIST;	break;
		case ptTriangleStrip:	d3dPri = D3DPT_TRIANGLESTRIP;	break;
		case ptTriangleFan:
								d3dPri = D3DPT_TRIANGLEFAN;		break;
		case ptLineStrip:		d3dPri = D3DPT_LINESTRIP;		break;
		case ptLineList:		
								d3dPri = D3DPT_LINELIST;		break;
		case ptPointList:		d3dPri = D3DPT_POINTLIST;		break;
	}
	return d3dPri;
}  // RenderSystemD3D::_InterpretMemPool

/*****************************************************************************/
/*	PrimitiveCache implementation
/*****************************************************************************/
_inl bool PrimitiveCache::Draw( BaseMesh& bm, 
								int firstPri, int numPri,
								int firstVert, int numVert )
{
	PrimitiveBuffer* pb = (PrimitiveBuffer*)bm.getDevHandle();
	if (!pb)
	//  find proper primitive buffer & assign it to mesh
	{
		if (bm.isQuadList())
		{
			pb = &pbQuad[(int)bm.getVertexFormat()]; 
		}
		else
		if (bm.isStatic())
		{
			pb = &pbStatic[(int)bm.getVertexFormat()]; 
		}
		else
		pb = &pbDynamic[(int)bm.getVertexFormat()];
		bm.setDevHandle( pb );
	}
	assert( pb );
	pb->Draw( bm, firstPri, numPri, firstVert, numVert );
	return true;
} // PrimitiveCache::Draw

_inl bool PrimitiveCache::Draw( BaseMesh& bm )
{
	PrimitiveBuffer* pb = (PrimitiveBuffer*)bm.getDevHandle();
	if (!pb)
	//  find proper primitive buffer & assign it to mesh
	{
		if (bm.isQuadList())
		{
			pb = &pbQuad[(int)bm.getVertexFormat()]; 
		}
		else
		if (bm.isStatic())
		{
			pb = &pbStatic[(int)bm.getVertexFormat()]; 
		}
		else
		{
			pb = &pbDynamic[(int)bm.getVertexFormat()];
		}
		bm.setDevHandle( pb );
	}
	assert( pb );
	pb->Draw( bm );
	return true;
} // PrimitiveCache::Draw

/*****************************************************************************/
/*	PrimitiveBuffer implementation
/*****************************************************************************/
_inl PrimitiveBuffer::PrimitiveBuffer()
{
	memset( this, 0, sizeof( PrimitiveBuffer ) );
	FVF = 0;
	vf  = vfUnknown; 
} // PrimitiveBuffer::PrimitiveBuffer

_inl bool PrimitiveBuffer::Draw( BaseMesh& bm, int firstPri, int numPri, 
							int firstVert, int numVert )
{
	if (numVert == 0) return false;

	int firstIdx	= bm.numPriToIdx( firstPri	);
	int numIdx		= bm.numPriToIdx( numPri	);

	ib->FillData( bm.getIndices(),		bm.getNInd(),	bm			);
	vb->FillData( bm.getVertexData(),	bm.getNVert(),	bm, stride	);

	ib->SetCurrent( bm.getVBPos() );
	vb->SetCurrent( FVF, stride );
	
	if (bm.isIndexed() || bm.isQuadList())
	{
#ifdef _DX9
		DX_CHK( VertexBuffer::dev->DrawIndexedPrimitive(	PriTypeDX( bm.getPriType() ),
															0, 0,
															numVert, 
															bm.getIBPos() + firstIdx,
															numPri ) );
	}
#else
        DX_CHK( VertexBuffer::dev->DrawIndexedPrimitive(	PriTypeDX( bm.getPriType() ),
															0, 
															numVert, 
															bm.getIBPos() + firstIdx,
															numPri ) );
	}
#endif
	else
	{
		DX_CHK( VertexBuffer::dev->DrawPrimitive(	PriTypeDX( bm.getPriType() ),
													bm.getVBPos() + firstVert, 
													numPri ) );
	}
	
	INC_COUNTER( Polygons, numPri );
	INC_COUNTER( Dips, 1 );
	
	return true;
} // PrimitiveBuffer::Draw

_inl bool PrimitiveBuffer::Draw( BaseMesh& bm )
{
	if (bm.getNVert() == 0) return false;

	ib->FillData( bm.getIndices(),		bm.getNInd(),	bm );
	vb->FillData( bm.getVertexData(),	bm.getNVert(),	bm, stride );


	ib->SetCurrent( bm.getVBPos() );
	vb->SetCurrent( FVF, stride );
	
	if (bm.isIndexed() || bm.isQuadList())
	{
#ifdef _DX9
		DX_CHK( VertexBuffer::dev->DrawIndexedPrimitive(	PriTypeDX( bm.getPriType() ),
															0, 0,
															bm.getNVert(), 
															bm.getIBPos(),
															bm.getNPri() ) );
	}
#else
        DX_CHK( VertexBuffer::dev->DrawIndexedPrimitive(	PriTypeDX( bm.getPriType() ),
															0, 
															bm.getNVert(), 
															bm.getIBPos(),
															bm.getNPri() ) );
	}
#endif
	else
	{
		DX_CHK( VertexBuffer::dev->DrawPrimitive(	PriTypeDX( bm.getPriType() ),
													bm.getVBPos(), 
													bm.getNPri() ) );
	}
	
	INC_COUNTER( Polygons, bm.getNPri() );
	INC_COUNTER( Dips, 1 );

	return true;
} // PrimitiveBuffer::Draw

_inl void PrimitiveBuffer::SetVertexFormat( VertexFormat vertF )
{
	vf		= vertF;
	FVF		= VertexFormatFVF( vf );
	stride	= Vertex::CalcStride( vf );
} // PrimitiveBuffer::SetVertexFormat

/*****************************************************************************/
/*	VertexBuffer implementation
/*****************************************************************************/
_inl void VertexBuffer::SetCurrent( DWORD fvf, int stride )
{
	assert( dev );
	
	if (softwareVP)
	{
		DX_CHK( dev->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, TRUE ) );
	}

	if (fvf != 0)
	{
		assert( D3DXGetFVFVertexSize( fvf ) == stride );
		DX_CHK( dev->SetVertexShader( fvf ) );
	}
#ifdef _DX9
	dev->SetStreamSource( 0, dxVB, 0, stride );
#else
    dev->SetStreamSource( 0, dxVB, stride );
#endif

	curFVF		= fvf;
	curVB		= this;
	curStride	= stride;
} // VertexBuffer::SetCurrent

/*****************************************************************************/
/*	IndexBuffer implementation
/*****************************************************************************/
_inl void IndexBuffer::SetCurrent( unsigned int baseIndex )
{
	assert( dev && dxIB );
	
	if (softwareVP)
	{
		DX_CHK( dev->SetRenderState( D3DRS_SOFTWAREVERTEXPROCESSING, TRUE ) );
	}
#ifdef _DX9
	dev->SetIndices( dxIB);
#else
    dev->SetIndices( dxIB, baseIndex );
#endif
} // IndexBuffer::SetCurrent





