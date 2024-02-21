/*****************************************************************************/
/*	File:	rsDX.inl
/*	Author:	Ruslan Shestopalyuk
/*	Date:	30.01.2003
/*****************************************************************************/

/*****************************************************************************/
/*	Render system DX utilities
/*****************************************************************************/
_inl DWORD FvfDX( VertexFormat vertFmt )
{
	switch (vertFmt)
	{
	case vf2Tex:	return D3DFVF_XYZ		| D3DFVF_DIFFUSE | D3DFVF_TEX2; 
	case vfTnL:		return D3DFVF_XYZRHW	| D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1;
	case vfTnL2:	return D3DFVF_XYZRHW	| D3DFVF_DIFFUSE | D3DFVF_TEX2;
	case vfN:		return D3DFVF_XYZ		| D3DFVF_NORMAL	 | D3DFVF_TEX1;
	case vfTnL2S:	return D3DFVF_XYZRHW	| D3DFVF_DIFFUSE |D3DFVF_SPECULAR | D3DFVF_TEX2;
	case vfT:		return D3DFVF_XYZ		| D3DFVF_TEX1;		
	case vfMP1:		return D3DFVF_XYZB1		| D3DFVF_LASTBETA_UBYTE4 | D3DFVF_TEX2;	
	case vfNMP1:	return D3DFVF_XYZB1 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX2;		
	case vfNMP2:	return D3DFVF_XYZB2 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX2;		
	case vfNMP3:	return D3DFVF_XYZB3 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX2;		
	case vfNMP4:	return D3DFVF_XYZB4 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX2;		
	default: return D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2; 
	
	}
	return D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX2;
} // FvfDX

_inl D3DFORMAT ColorFormatG2DX( ColorFormat colFmt )
{
	D3DFORMAT d3dcf = D3DFMT_A4R4G4B4;
	switch (colFmt)
	{
	case cfARGB4444: d3dcf = D3DFMT_A4R4G4B4;	break;
	case cfXRGB1555: d3dcf = D3DFMT_X1R5G5B5;	break;
	case cfARGB8888: d3dcf = D3DFMT_A8R8G8B8;	break;
	case cfXRGB8888: d3dcf = D3DFMT_X8R8G8B8;	break;
	case cfRGB565:   d3dcf = D3DFMT_R5G6B5;	    break;
	case cfV8U8:     d3dcf = D3DFMT_V8U8;	    break;
	case cfUnknown:	 d3dcf = D3DFMT_UNKNOWN;	break;
	}
	return d3dcf;
} // FvfDX

_inl ColorFormat ColorFormatDX2G( D3DFORMAT colFmt )
{
	ColorFormat res = cfARGB4444;
	switch (colFmt)
	{
	case D3DFMT_A4R4G4B4: res = cfARGB4444;	break;
	case D3DFMT_X1R5G5B5: res = cfXRGB1555;	break;
	case D3DFMT_A8R8G8B8: res = cfARGB8888;	break;
	case D3DFMT_X8R8G8B8: res = cfXRGB8888;	break;
	case D3DFMT_R5G6B5:   res = cfRGB565;	break;
	case D3DFMT_V8U8:     res = cfV8U8;	    break;
	case D3DFMT_UNKNOWN:  res = cfUnknown;	break;
	};
	return res;
} // ColorFormatDX

const D3DPOOL c_MemoryPoolDX[] = {	D3DPOOL_DEFAULT, 
									D3DPOOL_SYSTEMMEM, 
									D3DPOOL_DEFAULT, 
									D3DPOOL_MANAGED };

const MemoryPool c_MemoryPoolG[] = {	mpVRAM, 
									mpManaged, 
									mpSysMem, 
									mpUnknown,
									mpUnknown };

_inl D3DPOOL MemoryPoolG2DX( MemoryPool memPool )
{
	return c_MemoryPoolDX[(int)memPool];
}  // MemoryPoolDX

_inl MemoryPool MemoryPoolDX2G( D3DPOOL memPool )
{
	return c_MemoryPoolG[(int)memPool];
}  // MemoryPoolDX

const DWORD c_TextureUsagesDX[] = { 0, 0, 0, D3DUSAGE_RENDERTARGET, D3DUSAGE_DYNAMIC };

_inl DWORD TexUsageG2DX( TextureUsage texUsage )
{
	return c_TextureUsagesDX[(int)texUsage];
}  // TexUsageDX

_inl TextureUsage TexUsageDX2G( DWORD usage )
{
	if (usage & D3DUSAGE_RENDERTARGET)	return tuRenderTarget;
	if (usage & D3DUSAGE_DYNAMIC)		return tuDynamic;
	return tuUnknown;
}  // TexUsageDX
