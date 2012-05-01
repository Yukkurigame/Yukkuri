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

void ftDone();

struct Char{
	FT_Glyph gl;
	FT_Bitmap bm;
	FT_Glyph_Metrics metrics;
	signed long horiAdvance;
	signed long vertAdvance;
	int height;
	char id[40];
	FT_UInt index;
	int pic;
	Char( ) {
		horiAdvance = vertAdvance = 0;
		pic = 0;
	}
};

struct font_data {
	int fontHeight;			// Font height.
	int cellHeight;
	int cellWidth;
	int baselineY; 			// Offset from chell top to baseline
	int texture;
	Char* chars[CHARSIZE];
	FT_Face face;
	GLuint atlasHandle;

	//The init function will create a font of
	//of the height h from the file fname.
	bool load( const char * fname, unsigned int height );

	//Free all the resources assosiated with the font.
	void clean();

	Char* getChar( unsigned int );

	int getKerning( FT_UInt pervious, FT_UInt index );

	Texture* glTexture( Char *, int sw, int sh );

	//Calculate text size.
	void size( int* w, int* h, const char* str );

	//Print all characters to texture
	Texture* print( int cols, int rows );

};



#endif /* YUKKURIFONT_H_ */
