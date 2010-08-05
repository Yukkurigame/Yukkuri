/*
 * Font.cpp
 *
 *  Created on: 29.06.2010
 */

#include "Font.h"
#include "GraphicsTypes.h"

///Create a display list coresponding to the give character.
void make_dlist( FT_Face face, unsigned int ch, Char* letter, GLuint list_base, GLuint * tex_base )
{

	//FIXME: Когда-нибудь я это перепишу.
	//По текстуре на символ - не хорошо.

	//The first thing we do is get FreeType to render our character
	//into a bitmap.  This actually requires a couple of FreeType commands:

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

	//Use our helper function to get the widths of
	//the bitmap data that we will need in order to create
	//our texture.
	int width = next_p2( bitmap.width );
	int height = next_p2( bitmap.rows );

	letter->gl = glyph;
	letter->bm = bitmap;
	//letter->bgl = bitmap_glyph;
	//letter->left = bitmap_glyph->left;
	letter->top = bitmap_glyph->top;
	letter->horiAdvance = face->glyph->metrics.horiAdvance >> 6;
	letter->horiBearingY = face->glyph->metrics.horiBearingY >> 6;
	letter->vertAdvance = face->glyph->metrics.vertAdvance >> 6;
	letter->vertBearingY = face->glyph->metrics.vertBearingY >> 6;
	letter->height = bitmap.rows;

	//Allocate memory for the texture data.
	GLubyte* expanded_data = new GLubyte[ 2 * width * height ];

	//Here we fill in the data for the expanded bitmap.
	//Notice that we are using two channel bitmap (one for
	//luminocity and one for alpha), but we assign
	//both luminocity and alpha to the value that we
	//find in the FreeType bitmap.
	//We use the ?: operator so that value which we use
	//will be 0 if we are in the padding zone, and whatever
	//is the the Freetype bitmap otherwise.
	for( int j=0; j < height; j++ ){
		for( int i=0; i < width; i++ ){
			expanded_data[2*(i+j*width)] = 255;
			expanded_data[2*(i+j*width)+1] =
					(i>=bitmap.width || j>=bitmap.rows) ?
					0 : bitmap.buffer[i + bitmap.width*j];
		}
	}

	//Now we just setup some texture paramaters.
	glBindTexture( GL_TEXTURE_2D, tex_base[ch]);
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR );

	//Here we actually create the texture itself, notice
	//that we are using GL_LUMINANCE_ALPHA to indicate that
	//we are using 2 channel data.
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		  0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, expanded_data );

	//With the texture created, we don't need to expanded data anymore

	delete [] expanded_data;

	//So now we can create the display list
	glNewList(list_base+ch,GL_COMPILE);

	glBindTexture(GL_TEXTURE_2D, tex_base[ch]);

	//first we need to move over a little so that
	//the character has the right amount of space
	//between it and the one before it.
	glTranslatef(bitmap_glyph->left,0,0);

	//Now we move down a little in the case that the
	//bitmap extends past the bottom of the line
	//(this is only true for characters like 'g' or 'y'.
	glPushMatrix();
	glTranslatef(0,bitmap_glyph->top-bitmap.rows,0);

	//Now we need to account for the fact that many of
	//our textures are filled with empty padding space.
	//We figure what portion of the texture is used by
	//the actual character and store that information in
	//the x and y variables, then when we draw the
	//quad, we will only reference the parts of the texture
	//that we contain the character itself.
	float	x=(float)bitmap.width / (float)width,
			y=(float)bitmap.rows / (float)height;

	//Here we draw the texturemaped quads.
	//The bitmap that we got from FreeType was not
	//oriented quite like we would like it to be,
	//so we need to link the texture to the quad
	//so that the result will be properly aligned.
	glBegin(GL_QUADS);
	glTexCoord2d(0,0); glVertex2f(0,bitmap.rows);
	glTexCoord2d(0,y); glVertex2f(0,0);
	glTexCoord2d(x,y); glVertex2f(bitmap.width,0);
	glTexCoord2d(x,0); glVertex2f(bitmap.width,bitmap.rows);
	glEnd();
	glPopMatrix();
	glTranslatef(face->glyph->advance.x >> 6 ,0,0);


	//increment the raster position as if we were a bitmap font.
	//(only needed if you want to calculate text length)
	//glBitmap(0,0,0,0,face->glyph->advance.x >> 6,0,NULL);

	//Finish the display list
	glEndList();

	//Fix memory leak
	//FT_Done_Glyph(glyph);
}

void font_data::clean() {
	for( unsigned int i = 0; i< CHARSIZE; ++i ){
		FT_Done_Glyph( chars[i]->gl );
		delete chars[i];
	}
	glDeleteLists( list_base, CHARSIZE );
	glDeleteTextures( CHARSIZE, textures );
	delete [] textures;
}

bool font_data::load(const char * fname, unsigned int h) {

	char fs[4];
	sprintf( fs, "%d", h );
	debug( 3, "Loading font " +  (string)fname + ". Size: " + fs + "\n" );

	//Allocate some memory to store the texture ids.
	textures = new GLuint[CHARSIZE];

	this->h=h;

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
	FT_Set_Char_Size( face, h << 6, h << 6, 96, 96);

	//Encoding
	FT_Select_Charmap(face, FT_ENCODING_UNICODE);

	//Here we ask opengl to allocate resources for
	//all the textures and displays lists which we
	//are about to create.
	list_base = glGenLists(CHARSIZE);
	glGenTextures( CHARSIZE, textures );

	//This is where we actually create each of the fonts display lists.
	for( unsigned int i=0; i<CHARSIZE; ++i ){
		chars[i] = new Char();
		make_dlist( face, i, chars[i], list_base, textures );
	}

	//We don't need the face information now that the display
	//lists have been created, so we free the assosiated resources.
	FT_Done_Face( face );

	//Ditto for the library.
	FT_Done_FreeType( library );

	return true;
}

void font_data::print( Texture* tex, int* sw, int* sh, const char* str )
{
	GLuint* texture;
	int width, height, swidth, sheight;
	int lineheight;
	int textlen;
	std::vector< char* > lines;

	width = swidth = height = sheight = lineheight = 0;

	textlen = strlen( str );

	char text[textlen];
	strcpy( text, str );

	//Split lines by /n and calculate size;
	char* token;
	token = strtok( text, "\n");
	while( token != 0 ){
		//int tmpheight = 0;
		int tmpwidth = 0;
		char* line = token;
		for( unsigned int g = 0, e = strlen( line ); g < e; ++g ){
			Char* tmpc = chars[ static_cast<int>(text[0]) ];
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

	sheight = lineheight * lines.size() + 1 ;

	width = swidth;
	height = sheight;

	(*sw) = swidth;
	(*sh) = sheight;

	if( width && ( width & ( width - 1 ) ) )
		width = next_p2( width );
	if( height && ( height & ( height - 1 ) ) )
		height = next_p2( height );

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
							if( ++charnum >= textlen ) continue;
							hoffset += tmpc->horiAdvance;
						}
						tmpc = chars[ static_cast<int>( lines[linenum][charnum] ) ];
						chartop = tmpc->top - lineheight;
					}
					expanded_data[ 2 * ( i + j * width ) + 1 ] =
						( 	i - hoffset >= tmpc->bm.width ||
							j + chartop - voffset < 0 || j + chartop - voffset >= tmpc->bm.rows // ||
							) ?
							0 : tmpc->bm.buffer[ i - hoffset + tmpc->bm.width * ( j + chartop - voffset ) ];
				}
			}
		}

		(*sh) += ( numlines + 1 ) * lineheight/4; //Add subline space to sprite;
	}

	texture = new GLuint;

	//Now we just setup some texture paramaters.
	glBindTexture( GL_TEXTURE_2D, *texture );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR );

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
