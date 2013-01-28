/*
 * GLTextures.cpp
 *
 *  Created on: 17.07.2012
 *
 */


#include "graphics/render/GLTextures.h"
//#include "graphics/utils/sdl_graphics.h"
#include "graphics/utils/Image.h"
#include <map>

#include "hacks.h"
#include "debug.h"


namespace {

	std::map < std::string, Texture* > texturesCache;

}


void GLTextures::add( std::string name, Texture* texture )
{
	Texture* cached;
	if( !texture )
		return;
	cached = get( name );
	if( cached == texture )
		return;
	GLTextures::free( cached );
	texturesCache[name] = texture;
}


void GLTextures::free( Texture* tex )
{
	if( tex && tex->tex ){
		glDeleteTextures( 1, &tex->tex );
		glDeleteTextures( 1, &tex->normals );
		delete tex;
		tex = NULL;
	}
}


Texture* GLTextures::get( std::string name )
{
	if( name == "" )
		return NULL;

	std::map < std::string, Texture* >::iterator it;
	it = texturesCache.find(name);
	if( it != texturesCache.end() ){
		return it->second;
	}
	return NULL;
}


Texture* GLTextures::load( std::string name )
{
	Texture* tex = get( name );

	if( tex != NULL )
		return tex;

	int sw, sh, nw, nh;
	unsigned int surface = Image::load( name.c_str(), &sw, &sh );
	unsigned int normals = Image::load( ("normals/" + name).c_str(), &nw, &nh, false );

	if( !surface ){
		Debug::debug( Debug::GRAPHICS, name + " not loaded.\n" );
		return NULL;
	}

	tex = create( name, surface, sw, sh );

	if( normals ){
		Debug::debug( Debug::GRAPHICS, name + " normals loaded.\n" );
		tex->normals = normals;
	}


	/*
	Texture* tex;
	SDL_Surface* surface;
	SDL_Surface* normals;

	if( name == "" )
		return NULL;

	tex = get( name );

	if( !tex ){

		surface = SDLGraphics::LoadImage( name.c_str() );
		normals = SDLGraphics::LoadImage( ("normals/" + name).c_str(), false );


		if( !surface ){
			Debug::debug( Debug::GRAPHICS, name + " not loaded.\n" );
			return NULL;
		}

		if( normals )
			Debug::debug( Debug::GRAPHICS, name + " normals loaded.\n" );

		tex = new Texture();
		tex->tex = tex->normals = 0;
		GLint sifmt, nifmt;
		GLenum stfmt, ntfmt;
		SDLGraphics::CreateGlTexture( surface, &sifmt, &stfmt );
		SDLGraphics::CreateGlTexture( surface, &nifmt, &ntfmt );
		generate( &tex->tex, GL_TEXTURE_2D, sifmt, surface->w, surface->h,
					stfmt, GL_UNSIGNED_INT, surface->pixels );
		if( normals )
			generate( &tex->normals, GL_TEXTURE_2D, nifmt, normals->w, normals->h,
					ntfmt, GL_UNSIGNED_INT, normals->pixels );
		//tex->tex = SDLGraphics::CreateGlTexture( surface );
		//tex->normals = SDLGraphics::CreateGlTexture( normals );
		tex->w = surface->w;
		tex->h = surface->h;

		add( name, tex );

		if( surface )
			SDL_FreeSurface( surface );
		if( normals )
			SDL_FreeSurface( normals );
	}
	*/

	return tex;
}



Texture* GLTextures::create( std::string name, GLuint texture, int width, int height )
{
	Texture* tex = new Texture();
	tex->normals = 0;
	tex->tex = texture;
	tex->w = width;
	tex->h = height;
	add( name, tex );
	return tex;
}



/*	This function creates new opengl texture. If texture is exists it will be cleared.
 *	ahandle - pointer to texture id
 *	width, height - texture size
 *	returns boolean
 */
bool GLTextures::generate( GLuint* ahandle, GLenum target, GLint internalformat,
		GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
	// Texture generation and setup.
	if( ahandle == NULL ){
		Debug::debug( Debug::GRAPHICS, "Bad texture for generation.\n" );
		return false;
	}
	if( *ahandle == 0 )
		glGenTextures(1, ahandle);
	glBindTexture( target, *ahandle );
	glTexImage2D( target, 0, internalformat, width, height, 0, format, type, pixels );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( target, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glBindTexture( target, 0 );
	return true;
}



void GLTextures::clean( )
{
	FOREACHIT( texturesCache ){
		GLTextures::free( it->second );
		it->second = NULL;
	}
	texturesCache.clear();
}

