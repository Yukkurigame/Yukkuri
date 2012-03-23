/*
 * Font.h
 *
 *  Created on: 28.06.2010
 */

#ifndef YUKKURIFONT_H_
#define YUKKURIFONT_H_

#include "debug.h"
#include "GraphicsTypes.h"

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#define CHARSIZE 256

struct Char{
	FT_Glyph gl;
	FT_Bitmap bm;
	signed long horiAdvance;
	signed long vertAdvance;
	int top;
	int height;
	Char( ) { horiAdvance = vertAdvance = 0; top = height = 0; }
};

struct font_data {
	int fontHeight;			// Font height.
	Char* chars[CHARSIZE];

	//The init function will create a font of
	//of the height h from the file fname.
	bool load( const char * fname, unsigned int height );

	//Free all the resources assosiated with the font.
	void clean();

	//Calculate text size.
	void size( int* w, int* h, const char* str );


	//Print text string to sprite
	void print( Sprite* spr, int* sw, int* sh, const char* text );
};



#endif /* YUKKURIFONT_H_ */
