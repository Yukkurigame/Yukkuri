/*
 * Textures.h
 *
 *  Created on: 26.01.2013
 */

#ifndef TEXTURES_H_
#define TEXTURES_H_

#include "types.h"
#include "graphics/GraphicsTypes.h"
#include "utils/list.h"


namespace Textures {

	void clean( );

	inline bool create( GLuint* ahandle, int width, int height ){
		return create( ahandle, GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_INT, NULL );
	}
	inline bool create( GLuint* ahandle, int width, int height, const void* data ){
		return create( ahandle, GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_INT, data );
	}
	bool create( GLuint* ahandle, GLenum target, GLint internalformat, GLsizei width, GLsizei height,
					GLenum format, GLenum type, const GLvoid *pixels );

	UINT get_active( UINT id );

	TextureInfo* get_pointer( UINT number );


	//////////////////////////////////////////////////
	// Textures

	bool LoadTextures( );

	int push( TextureProxy*, GLuint atlas, GLuint normals );
	void push( list< TextureProxy* >& tarray, GLuint atlas, GLuint normals );

	int GetTextureNumberById( std::string id );


	bool DrawToGLTexture( GLuint ahandle, list< TextureProxy* >* textures );

}



#endif /* TEXTURES_H_ */
