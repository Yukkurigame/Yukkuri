/*
 * GLTextures.h
 *
 *  Created on: 17.07.2012
 *
 */

#ifndef GLTEXTURES_H_
#define GLTEXTURES_H_

#include "graphics/GraphicsTypes.h"
#include "graphics/sprite/Sprite.h"
#include "utils/list.h"
#include <string>


namespace GLTextures
{

	void add( std::string name, Texture* texture );
	void free( Texture* tex );
	void clean( );

	Texture* get( std::string name );
	Texture* load( std::string name );
	Texture* create( std::string name, GLuint texture, int width, int height );

	bool generate( GLuint* ahandle, GLenum target, GLint internalformat, GLsizei width, GLsizei height,
					GLenum format, GLenum type, const GLvoid *pixels );
	inline bool generate( GLuint* ahandle, int width, int height, GLenum type, const void* data ){
		return generate( ahandle, GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, type, data );
	}
	inline bool generate( GLuint* ahandle, int width, int height ){
		return generate( ahandle, GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_INT, NULL );
	}
	inline bool generate( Texture* t ){
		return generate( &t->tex, t->w, t->h );
	}



	bool draw( GLuint* ahandle, int width, int height, list< Sprite* >* sprites, bool invert = false );

	bool update( GLuint* ahandle, int width, int height, Sprite* sprites, bool invert = false );
	inline bool update( Texture* tex, Sprite* sprite, bool invert = false ){
		return update( &tex->tex, tex->w, tex->h, sprite, invert );
	}
}


#endif /* GLTEXTURES_H_ */
