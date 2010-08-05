/*
 * Font.h
 *
 *  Created on: 28.06.2010
 */

#ifndef YUKKURIFONT_H_
#define YUKKURIFONT_H_

#include "debug.h"

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include "GraphicsTypes.h"

#include <vector>
#include <string>
using std::string;

#define CHARSIZE 256

inline int next_p2( int a )
{
	int rval=2;
	while(rval<a) rval<<=1;
	return rval;
}

struct Char{
	//FIXME: cleanup
	FT_Glyph gl;
	FT_Bitmap bm;
	signed long horiAdvance;
	signed long horiBearingY;
	signed long vertAdvance;
	signed long vertBearingY;
	int top;
	//int left;
	int height;
	Char( ) { horiAdvance = vertAdvance = vertBearingY = 0; top = height = 0; }
};

struct font_data {
	float h;			// Font height.
	GLuint * textures;	// Texture id's
	GLuint list_base;	// First display list id
	Char* chars[CHARSIZE];


	//The init function will create a font of
	//of the height h from the file fname.
	bool load( const char * fname, unsigned int h );

	//Free all the resources assosiated with the font.
	void clean();

	//Print text string to sprite
	void print( Texture* spr, int* sw, int* sh, const char* text );
};



#endif /* YUKKURIFONT_H_ */
