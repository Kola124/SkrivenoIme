/*****************************************************************/
/*  File:   gpText.h
/*  Desc:   Bitmap text drawn on the quads
/*  Author: Silver, Copyright (C) GSC Game World
/*  Date:   Mar 2002
/*****************************************************************/
#ifndef __GPTEXT_H__
#define __GPTEXT_H__

#include "IRenderSystem.h"

const int c_MaxTextSymbols = 512;

enum GPTextAlignment
{
	taUnknown		= 0,
	taLeftTop		= 1,
	taLeftBottom	= 2,
	taRightTop		= 3,
	taRightBottom	= 4,
	tsMiddleTop		= 5,
	tsMiddleBottom	= 6
}; // enum GPTextAlignment

class IRenderSystem;
/*****************************************************************/
/*  Class:	GPFont
/*  Desc:	Class of the bitmap text drawn on the 2D quads
/*****************************************************************/
class GPFont 
{
public:
	GPFont();
	~GPFont();

	void	Init( const char* fontName, int	fontHeight = 8 );
	void	DrawString( const char* str, 
						float x, float y, float z,
						DWORD color		= 0xFFFFFFFF,	//  color of text
						float height	= -1.0f,		//  height of text, in pixels
						Camera* cam		= NULL,			//  when NULL text position is taken in screen space
						GPTextAlignment align = taLeftTop );
private:
	void				CreateSymbolTable( const char* fontName, int _fontHeight );

	int					nMaxTextSymbols;
	float				posX, posY, depth;	//  coords of the text begin
	BaseMesh			mesh;

	int					texID;
	int					fontHeight;
	float				texCoord[96][4];

	String				txt;
};  // class GPFont

#endif // __GPTEXT_H__