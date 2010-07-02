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
#include "SDL_opengl.h"
//#include <GL/gl.h>
//#include <GL/glu.h>

#include <vector>
#include <string>
using std::string;

struct font_data {
	float h;			// Font height.
	GLuint * textures;	// Texture id's
	GLuint list_base;	// First display list id

	//The init function will create a font of
	//of the height h from the file fname.
	bool load( const char * fname, unsigned int h );

	//Free all the resources assosiated with the font.
	void clean();
};


#endif /* YUKKURIFONT_H_ */
