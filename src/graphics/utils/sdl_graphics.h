/*
 * sdl_graphics.h
 *
 *  Created on: 20.03.2012
 */

#ifndef SDL_GRAPHICS_H_
#define SDL_GRAPHICS_H_

//#include "Define.h"
//#include "SDL/SDL.h"

/*
namespace SDLGraphics {

	bool SetScreen( SDL_Surface* s );
	SDL_Surface* GetScreen();

	void CreateGlTexture( SDL_Surface*, GLint*, GLenum* );

	SDL_Surface* LoadImage( const char*, bool force=true );
	SDL_Surface* OpenImage( const char* );

	void DrawSurface( SDL_Surface* surface );


}
*/

namespace Screenshot
{
	bool Save( );
	/* Generate screenshot name. */
	int Exists( char *name );
	void GenerateName( char* name );
}



#endif /* SDL_GRAPHICS_H_ */
