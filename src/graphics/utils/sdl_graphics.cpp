/*
 * sdl_graphics.cpp
 *
 *  Created on: 20.03.2012
 */
#include <cstdlib>

#include "Define.h"

#include "sdl_graphics.h"
#include "SDL/SDL_image.h"
#include "safestring.h"
#include "config.h"
#include "debug.h"

using namespace Debug;

#include <sys/stat.h>
#if WIN32
extern "C" {
#endif
	#include "pngfuncs.h"
#if WIN32
}
#endif


extern MainConfig conf;

namespace {

	SDL_Surface* screen = NULL;

}


bool SDLGraphics::SetScreen( SDL_Surface* s ){
	if( s == NULL )
		return false;
	screen = s;
	return true;
}


SDL_Surface* SDLGraphics::GetScreen() {
	return screen;
}



GLuint SDLGraphics::CreateGlTexture( SDL_Surface* surface )
{
	GLuint image;
	GLint  nOfColors;
	GLenum texture_format = 0;

	if( surface == NULL )
		return 0;

	/*if( surface->format->colorkey != 0 ){
		//FIXME: Indexed images support.
		debug( 1, "BIDA while loading " + name + "! Indexed images not supported yet!\n" );
		texture_format = GL_COLOR_INDEX;
	}else{*/
		nOfColors = surface->format->BytesPerPixel;

		if( nOfColors == 4 ){     // contains an alpha channel
			if( surface->format->Rmask == 0x000000ff )
				texture_format = GL_RGBA;
			else
				texture_format = GL_BGRA;
		}else if( nOfColors == 3 ){     // no alpha channel
			if (surface->format->Rmask == 0x000000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		}else{
			debug( GRAPHICS, "Image is not truecolor.. this will probably break.\n" );
			// this error should not go unhandled
		}
	//}

	// Have OpenGL generate a texture object handle for us
	glGenTextures( 1, &image );

	// Bind the texture object
	glBindTexture( GL_TEXTURE_2D, image );

	// Set the texture's stretching properties
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// Edit the texture object's image data using the information SDL_Surface gives us
	glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
					texture_format, GL_UNSIGNED_BYTE, surface->pixels );

	return image;

}


SDL_Surface* SDLGraphics::LoadImage( const char* name, bool force )
{
	SDL_Surface* pImg = NULL;
	char* path = (char*)malloc( (unsigned int)conf.imagePath.size( ) + strlen( name ) + 1 );
	strcpy( path, conf.imagePath.c_str() );
	strcat( path, name );
	pImg = OpenImage( path );
	if( !pImg && force ){
		//Not loaded.
		if( strcmp( name, conf.defaultImage.c_str() ) ){
			strcpy( path, conf.defaultImage.c_str() );
			pImg = LoadImage( path );
		}else{
			free(path);
			return NULL; // Default already tried. Break.
		}
	}
	free(path);
	return pImg;
}


SDL_Surface* SDLGraphics::OpenImage( const char* filename )
{
	SDL_Surface* loadedImage = NULL;
	SDL_Surface* optimizedImage = NULL;
	int string_size = sizeof(char) * ( strlen(filename) + 30 );
	char* dbg = (char*)malloc( string_size );
	loadedImage = IMG_Load( filename );

	if( loadedImage != NULL ){
		if( ( loadedImage->w & (loadedImage->w - 1) ) != 0 ){
			snprintf( dbg, string_size, "%s width is not a power of 2!\n", filename );
			debug( GRAPHICS, dbg );
		}else if( (loadedImage->h & (loadedImage->h - 1) ) != 0 ){
			snprintf( dbg, string_size, "%s height is not a power of 2!\n", filename );
			debug( GRAPHICS, dbg );
		}else{
			//FIXME: no-alpha images
			if( loadedImage->format->Amask )
				optimizedImage = SDL_DisplayFormatAlpha( loadedImage );
			else
				optimizedImage = SDL_DisplayFormatAlpha( loadedImage );
		}
		SDL_FreeSurface( loadedImage );
	}else{
		snprintf( dbg, string_size, "Could not load %s.\n", filename );
		debug( GRAPHICS, dbg );
	}
	free(dbg);
	return optimizedImage;
}


void SDLGraphics::DrawSurface( SDL_Surface* surface )
{

	int bytes;

	if( !surface )
		return;

	bytes = surface->pitch / surface->w;

	glRasterPos3i( 0, 0, 1 );

	//glDisable( GL_TEXTURE_2D );

	switch( bytes ){
		case 4:
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			glDrawPixels( surface->w, surface->h, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, surface->pixels );
		#else
			glDrawPixels( surface->w, surface->h, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels );
		#endif
			break;
		case 3:
			glDrawPixels( surface->w, surface->h, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels );
			break;
		case 2:
			glDrawPixels( surface->w, surface->h, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, surface->pixels );
			break;
	}

	//glEnable( GL_TEXTURE_2D );

}


bool Screenshot::Save( )
{
	char Filename[52];
	int stride;
	GLubyte* pixels;
	GLubyte* swapline;
	Uint32 rmask, gmask, bmask, amask;
	SDL_Surface* output;
	SDL_Surface* screen;

	GenerateName( Filename );
	if( Filename[0] == '\0' ){
		debug( OS, "Can not get screenshot name. Too many screenshots in folder.\n" );
		return false;
	}

	screen = SDLGraphics::GetScreen();
	if( screen == NULL ){
		debug( GRAPHICS, "Screen not exists.\n" );
		return false;
	}

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000; gmask = 0x00ff0000; bmask = 0x0000ff00; amask = 0x000000ff;
	#else
	rmask = 0x000000ff; gmask = 0x0000ff00; bmask = 0x00ff0000; amask = 0xff000000;
	#endif


	stride = screen->w * 4;
	pixels = (GLubyte *) malloc( stride * screen->h );
	swapline = (GLubyte *) malloc( stride );
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, screen->w, screen->h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	// vertical flip
	for( int row = 0; row < screen->h/2; ++row ){
		memcpy( swapline, pixels + row * stride, stride );
		memcpy( pixels + row * stride, pixels + ( screen->h - row - 1 ) * stride, stride );
		memcpy( pixels + ( screen->h - row -1 ) * stride, swapline, stride );
	}
	free( swapline );

	output = SDL_CreateRGBSurfaceFrom( pixels, screen->w, screen->h, 32, screen->pitch,
										rmask, gmask, bmask, amask );

	if( png_save_surface(Filename, output) < 0 ){
		free( pixels );
		SDL_FreeSurface( output );
		return false;
	}
	free( pixels );
	SDL_FreeSurface( output );
	return true;
}

int Screenshot::Exists( char *name )
{
	struct stat dummy;
	if( stat( name, &dummy ) == 0 )
		return 1;
	return 0;
}

void Screenshot::GenerateName( char* name )
{
	int iter = 0;
	do{
		snprintf( name, 50, "screenshot%03d.png", ++iter );
	}while( Exists( name ) && iter < 1000 );
	if( Exists( name ) ){
		name[0] = '\0';
	}
}
