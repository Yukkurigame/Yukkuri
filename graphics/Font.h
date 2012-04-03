/*
 * Font.h
 *
 *  Created on: 28.06.2010
 */

#ifndef YUKKURIFONT_H_
#define YUKKURIFONT_H_

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
	FT_Glyph_Metrics metrics;
	signed long horiAdvance;
	signed long vertAdvance;
	int top;
	int height;
	char id[40];
	unsigned int chr;
	int pic;
	Char( ) {
		horiAdvance = vertAdvance = 0; top = height = 0;
		pic = 0;
	}
};

struct font_data {
	int fontHeight;			// Font height.
	int cellHeight;
	int cellWidth;
	int texture;
	Char* chars[CHARSIZE];

	//The init function will create a font of
	//of the height h from the file fname.
	bool load( const char * fname, unsigned int height );

	//Free all the resources assosiated with the font.
	void clean();

	Char* getChar( unsigned int );

	Texture* glTexture( Char *, int sw, int sh );

	//Calculate text size.
	void size( int* w, int* h, const char* str );


	//Print text string to sprite
	void print( Sprite* spr, int* sw, int* sh, const char* text );
};



#endif /* YUKKURIFONT_H_ */
