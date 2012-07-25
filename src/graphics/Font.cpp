/*
 * Font.cpp
 *
 *  Created on: 29.06.2010
 */

#include "Font.h"
#include "Render.h"
#include "graphics/render/Atlas.h"

#include "safestring.h"
#include "debug.h"
#include "hacks.h"
using namespace Debug;


#include <vector>

namespace {

	FT_Library library;
	bool ftLoaded = false;

	int lastLine = 0;

	inline int next_p2( int a )
	{
		int rval=2;
		while( rval < a ) rval <<= 1;
		return rval;
	}

	///Create a display list coresponding to the give character.
	void makeChar( FT_Face face, unsigned int ch, Char* letter )
	{
		FT_UInt index = FT_Get_Char_Index( face, ch );

		//Load the Glyph for our character.
		if( FT_Load_Glyph( face, index, FT_LOAD_DEFAULT ) ){
			debug( GRAPHICS, "FT_Load_Glyph failed\n" );
			return;
		}

		//Move the face's glyph into a Glyph object.
		FT_Glyph glyph;
		if(FT_Get_Glyph( face->glyph, &glyph )){
			debug( GRAPHICS, "FT_Get_Glyph failed\n" );
			return;
		}

		//Convert the glyph to a bitmap.
		FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
		FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

		//This reference will make accessing the bitmap easier
		FT_Bitmap& bitmap = bitmap_glyph->bitmap;

		letter->gl = glyph;
		letter->bm = bitmap;
		letter->horiAdvance = face->glyph->advance.x >> 6;
		letter->vertAdvance = face->glyph->metrics.vertAdvance >> 6;
		letter->metrics = face->glyph->metrics;
		letter->height = bitmap.rows;
		letter->index = index;

	}
}


void ftDone(  )
{
	if( !ftLoaded )
		return;
	//Ditto for the library.
	FT_Done_FreeType( library );
	ftLoaded = false;
}


void font_data::clean() {
	for( unsigned int i = 0; i< CHARSIZE; ++i ){
		FT_Done_Glyph( chars[i]->gl );
		delete chars[i];
	}
}


bool font_data::load( const char * fname, unsigned int height ) {

	char fs[4];
	snprintf( fs, 4, "%d", height );
	debug( GRAPHICS, "Loading font " +  (std::string)fname + ". Size: " + fs + "\n" );

	//Allocate some memory to store the texture ids.
	//textures = new GLuint[CHARSIZE];

	this->fontHeight = height;

	//Create and initilize a freetype font library.
	if( !ftLoaded ){
		if( FT_Init_FreeType( &library ) ){
			debug( GRAPHICS, "FT_Init_FreeType failed in " +  (std::string)fname + "\n" );
			return false;
		}
		ftLoaded = true;
	}

	//This is where we load in the font information from the file.
	//Of all the places where the code might die, this is the most likely,
	//as FT_New_Face will die if the font file does not exist or is somehow broken.
	if( FT_New_Face( library, fname, 0, &face ) ){
		debug( GRAPHICS, "FT_New_Face failed in " + (std::string)fname + " (there is probably a problem with your font file)\n" );
		return false;
	}

	//For some twisted reason, Freetype measures font size
	//in terms of 1/64ths of pixels.  Thus, to make a font
	//h pixels high, we need to request a size of h*64.
	//(h << 6 is just a prettier way of writting h*64)
	FT_Set_Char_Size( face, height << 6, height << 6, 96, 96);

	//Encoding
	FT_Select_Charmap(face, FT_ENCODING_UNICODE);

	//This is where we actually create each of the fonts display lists.
	for( unsigned int i=0; i<CHARSIZE; ++i ){
		chars[i] = new Char();
		makeChar( face, i, chars[i] );
		chars[i]->pic = i;
	}

	// Calculate texture params
	FT_BBox bbox = face->bbox;
	cellWidth = (bbox.xMax - bbox.xMin) >> 6;
	cellHeight = (bbox.yMax - bbox.yMin) >> 6;
	baselineY = bbox.yMax >> 6;

	int w, h;
	w = h = TextureAtlas::getAtlasMax();
	int sw = (w / cellWidth) * cellWidth; // make width multiple
	int cols = w / cellWidth;
	if( cols < 1 ) cols = 1;
	int rows = 1 + CHARSIZE / cols;
	int sh = rows * cellHeight;

	// Draw new line into atlas;
	Texture* tex = print( cols, rows );
	int namelen = strlen( face->family_name ) + strlen( face->style_name ) + 7;
	char* name = new char[namelen];
	snprintf( name, namelen - 1, "%s %s %3d", face->family_name, face->style_name, height );
	// Add texture to render manager
	TextureAtlas::addTexture( name, tex, sw, sh, cols, rows, 0, lastLine );
	// Build texture atlas
	TextureAtlas::create( &atlasHandle, w, h );
	lastLine += rows * cellHeight;
	delete tex;

	// Get texture id for font
	texture = RenderManager::GetTextureNumberById( name );

	delete[] name;

	return true;
}




void font_data::size( int* w, int* h, const char* str )
{
	int swidth;
	int lineheight;
	int nlines;

	nlines = swidth = lineheight = 0;

	char* text = strdup( str );

	char* token;
	token = strtok( text, "\n" );
	while( token != 0 ){
		int tmpwidth = 0;
		char* line = token;
		for( unsigned int g = 0, e = strlen( line ); g < e; ++g ){
			Char* tmpc = chars[ static_cast<int>(text[g]) ];
			tmpwidth += tmpc->horiAdvance;
			if( tmpc->height > lineheight ){
				lineheight = tmpc->vertAdvance;
			}
		}
		if( tmpwidth > swidth )
			swidth = tmpwidth;
		++nlines;
		token = strtok(NULL, "\n");
	}
	free(text);

	if( w )
		(*w) = swidth;
	if( h )
		(*h) = ( lineheight + lineheight/4 ) * ( nlines - 1 );
}


Char* font_data::getChar( unsigned int c )
{
	//TODO: unicode support
	if( c < 0 || c > CHARSIZE )
		return getChar('?');
	return chars[ c ];
}

int font_data::getKerning( FT_UInt pervious, FT_UInt index )
{
	FT_Vector delta;
	delta.x = 0;

	if( FT_HAS_KERNING(face) && pervious && index ){
		FT_Get_Kerning( face, pervious, index, FT_KERNING_DEFAULT, &delta );
	}

	return delta.x >> 6;
}

Texture* font_data::glTexture( Char* ch, int sw, int sh )
{
	Texture* tex;
	GLuint texture;
	int width, height, swidth, sheight;

	if( ch == NULL )
		return NULL;

	width = swidth = height = sheight = 0;

	swidth = sh;
	sheight = sw + sw/4;

	width = next_p2( swidth );
	height = next_p2( sheight );

	//Allocate memory for the texture data.
	GLubyte* expanded_data = new GLubyte[ 2 * width * height ];

	// it was a good code but it was rewritten
	{
		int linenum = 0;
		int numlines = 1;
		for( int j=0; j < height; ++j ){
			if( j >= sheight ){ //line up
				linenum++;
			}
			for( int i=0; i < width; ++i ){
				expanded_data[ 2 * ( i + j * width ) ] = 255; //Alpha
				if( linenum >= numlines ){
					expanded_data[ 2 * ( i + j * width ) + 1 ] = 0;
				}else{
					if( i >= swidth ){
						expanded_data[ 2 * ( i + j * width ) + 1 ] = 0;
						continue;
					}
					expanded_data[ 2 * ( i + j * width ) + 1 ] =
						( 	i >= ch->bm.width || j < 0 || j >= ch->bm.rows
							) ? 0 : ch->bm.buffer[ i + ch->bm.width * j ];
				}
			}
		}
	}

	glGenTextures( 1, &texture );

	//Now we just setup some texture paramaters.
	glBindTexture( GL_TEXTURE_2D, texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	//Here we actually create the texture itself, notice
	//that we are using GL_LUMINANCE_ALPHA to indicate that
	//we are using 2 channel data.
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data );

	//With the texture created, we don't need to expanded data anymore

	delete [] expanded_data;

	tex = new Texture();
	tex->tex = texture;
	tex->w = width;
	tex->h = height;

	return tex;
}


Texture* font_data::print( int cols, int rows )
{
	Texture* texture;
	int width, height, swidth, sheight;
	GLuint tex = 0;

	swidth = cols * cellWidth;
	sheight = rows * cellHeight;

	width = next_p2( swidth );
	height = next_p2( sheight );

	//Allocate memory for the texture data.
	GLubyte* expanded_data = new GLubyte[ 2 * width * height ];

	{
		int line = 0;
		int voffset = 0;
		int charoffset = 0;
		for( int j=0; j < height; j++ ){
			Char* tmpc = NULL;
			int col = 0;
			int hoffset = 0;
			int chartop = 0;
			int charleft = 0;
			if( j >= cellHeight + voffset ){ //line up
				line++;
				voffset += cellHeight;
				charoffset += cols;
			}
			for( int i=0; i < width; i++ ){
				expanded_data[ 2 * ( i + j * width ) ] = 255; //Alpha
				if( col >= cols || line >= rows || charoffset + col >= CHARSIZE ){
					expanded_data[ 2 * ( i + j * width ) + 1 ] = 0;
				}else{
					if( !tmpc || i >= hoffset + cellWidth ){ //symbol up;
						if( tmpc ){
							if( ++col >= cols || charoffset + col >= CHARSIZE ){
								expanded_data[ 2 * ( i + j * width ) + 1 ] = 0;
								continue;
							}
							hoffset += cellWidth;
						}
						tmpc = chars[ charoffset + col ];
						chartop = baselineY - (tmpc->metrics.horiBearingY  >> 6);
						charleft = tmpc->metrics.horiBearingX >> 6;
					}
					expanded_data[ 2 * ( i + j * width ) + 1 ] =
						( 	i - charleft - hoffset >= tmpc->bm.width ||
							i - charleft - hoffset < 0 ||
							j - chartop - voffset < 0 ||
							j - chartop - voffset >= tmpc->bm.rows
							) ?
							0 : tmpc->bm.buffer[ i - charleft - hoffset + tmpc->bm.width * ( j - voffset - chartop ) ];
				}
			}
		}
	}

	glGenTextures( 1, &tex );

	//Now we just setup some texture paramaters.
	glBindTexture( GL_TEXTURE_2D, tex );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	//Here we actually create the texture itself, notice
	//that we are using GL_LUMINANCE_ALPHA to indicate that
	//we are using 2 channel data.
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data );

	//With the texture created, we don't need to expanded data anymore

	delete [] expanded_data;

	texture = new Texture();

	texture->tex = tex;
	texture->w = width;
	texture->h = height;

	return texture;

}
