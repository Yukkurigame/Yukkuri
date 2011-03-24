/*
 * Font.cpp
 *
 *  Created on: 29.06.2010
 */

#include "Font.h"

#include "safestring.h"
using std::string;

#include <vector>


inline int next_p2( int a )
{
	int rval=2;
	while( rval < a ) rval <<= 1;
	return rval;
}

///Create a display list coresponding to the give character.
void makeChar( FT_Face face, unsigned int ch, Char* letter )
{

	//Load the Glyph for our character.
	if( FT_Load_Glyph( face, FT_Get_Char_Index( face, ch ), FT_LOAD_DEFAULT ) ){
		debug(4, "FT_Load_Glyph failed\n");
		return;
	}

	//Move the face's glyph into a Glyph object.
	FT_Glyph glyph;
	if(FT_Get_Glyph( face->glyph, &glyph )){
		debug(4, "FT_Get_Glyph failed\n");
		return;
	}

	//Convert the glyph to a bitmap.
	FT_Glyph_To_Bitmap( &glyph, ft_render_mode_normal, 0, 1 );
	FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

	//This reference will make accessing the bitmap easier
	FT_Bitmap& bitmap=bitmap_glyph->bitmap;

	letter->gl = glyph;
	letter->bm = bitmap;
	letter->top = bitmap_glyph->top;
	letter->horiAdvance = face->glyph->metrics.horiAdvance >> 6;
	letter->vertAdvance = face->glyph->metrics.vertAdvance >> 6;
	letter->height = bitmap.rows;

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
	debug( 3, "Loading font " +  (string)fname + ". Size: " + fs + "\n" );

	//Allocate some memory to store the texture ids.
	//textures = new GLuint[CHARSIZE];

	this->fontHeight = height;

	//Create and initilize a freetype font library.
	FT_Library library;
	if( FT_Init_FreeType( &library ) ){
		debug(3, "FT_Init_FreeType failed in " +  (string)fname + "\n");
		return false;
	}

	//The object in which Freetype holds information on a given
	//font is called a "face".
	FT_Face face;

	//This is where we load in the font information from the file.
	//Of all the places where the code might die, this is the most likely,
	//as FT_New_Face will die if the font file does not exist or is somehow broken.
	if( FT_New_Face( library, fname, 0, &face ) ){
		debug( 3, "FT_New_Face failed in " + (string)fname + " (there is probably a problem with your font file)\n" );
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
	}

	//We don't need the face information now that the display
	//lists have been created, so we free the assosiated resources.
	FT_Done_Face( face );

	//Ditto for the library.
	FT_Done_FreeType( library );

	return true;
}

void font_data::size( int* w, int* h, const char* str )
{
	int swidth;
	int textlen;
	int lineheight;
	int nlines;

	nlines = swidth = lineheight = 0;
	textlen = strlen( str );

	char* text = (char*)malloc( sizeof(char)*textlen + 1 );
	strcpy( text, str );

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

void font_data::print( Texture* tex, int* sw, int* sh, const char* str )
{
	GLuint* texture;
	int width, height, swidth, sheight;
	int lineheight;
	int textlen;
	std::vector< char* > lines;

	if( !tex || str == NULL)
		return;

	width = swidth = height = sheight = lineheight = 0;

	textlen = strlen( str );

	char* text = (char*)malloc( sizeof(char) * ( textlen + 1 ) );
	strcpy( text, str );

	//Split lines by /n and calculate size;
	char* token;
	token = strtok( text, "\n");
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
		lines.push_back(line);
		token = strtok(NULL, "\n");
	}

	sheight = ( lineheight + lineheight/4 ) * lines.size();


	(*sw) = swidth;
	(*sh) = sheight;

	width = next_p2( swidth );
	height = next_p2( sheight );

	//Allocate memory for the texture data.
	GLubyte* expanded_data = new GLubyte[ 2 * width * height ];

	{
		int linenum = 0;
		int voffset = 0;
		int numlines = lines.size();
		textlen = strlen(lines[linenum]);
		for( int j=0; j < height; j++ ){
			Char* tmpc = NULL;
			int charnum = 0;
			int hoffset = 0;
			int chartop = 0;
			if( j >= lineheight + voffset + lineheight/4 ){ //line up
				linenum++;
				voffset += lineheight + lineheight/4;
				if( linenum < numlines )
					textlen = strlen(lines[linenum]);
			}
			for( int i=0; i < width; i++ ){
				expanded_data[ 2 * ( i + j * width ) ] = 255; //Alpha
				if( charnum >= textlen || linenum >= numlines ){
					expanded_data[ 2 * ( i + j * width ) + 1 ] = 0;
				}else{
					if( !tmpc || i >= hoffset + tmpc->horiAdvance ){ //symbol up;
						if( tmpc ){
							if( ++charnum >= textlen ){
								expanded_data[ 2 * ( i + j * width ) + 1 ] = 0;
								continue;
							}
							hoffset += tmpc->horiAdvance;
						}
						tmpc = chars[ static_cast<int>( lines[linenum][charnum] ) ];
						chartop = tmpc->top - lineheight;
					}
					expanded_data[ 2 * ( i + j * width ) + 1 ] =
						( 	i - hoffset >= tmpc->bm.width ||
							j + chartop - voffset < 0 ||
							j + chartop - voffset >= tmpc->bm.rows
							) ?
							0 : tmpc->bm.buffer[ i - hoffset + tmpc->bm.width * ( j + chartop - voffset ) ];
				}
			}
		}
	}

	free(text);

	texture = new GLuint;
	glGenTextures( 1, texture );

	//Now we just setup some texture paramaters.
	glBindTexture( GL_TEXTURE_2D, *texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	//Here we actually create the texture itself, notice
	//that we are using GL_LUMINANCE_ALPHA to indicate that
	//we are using 2 channel data.
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		  0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data );

	//With the texture created, we don't need to expanded data anymore

	delete [] expanded_data;

	tex->texture = texture;
	tex->w = width;
	tex->h = height;

}
