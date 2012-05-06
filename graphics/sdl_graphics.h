/*
 * sdl_graphics.h
 *
 *  Created on: 20.03.2012
 */

#ifndef SDL_GRAPHICS_H_
#define SDL_GRAPHICS_H_

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"


namespace SDLGraphics {

	GLuint CreateGlTexture( SDL_Surface* );

	SDL_Surface* LoadImage( const char* );
	SDL_Surface* OpenImage( const char* );

	void DrawSurface( SDL_Surface* surface );

	static SDL_Surface* screen;

	inline bool SetScreen( SDL_Surface* s ){
		if( s == NULL )
			return false;
		screen = s;
		return true;
	}
}

namespace Screenshot
{
	bool Save( );
	/* Generate screenshot name. */
	int Exists( char *name );
	void GenerateName( char* name );
}



#endif /* SDL_GRAPHICS_H_ */
