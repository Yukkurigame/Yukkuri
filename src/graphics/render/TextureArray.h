/*
 * TextureArray.h
 *
 *  Created on: 28.05.2012
 *
 */

#ifndef TEXTUREARRAY_H_
#define TEXTUREARRAY_H_

#include "graphics/sprite/Sprite.h"
#include <vector>

namespace TextureArray
{
	// Vertex buffer object
	VBOStructureHandle* prepareVBO( int pass, std::vector< Sprite* >& sprites );
	VBOStructureHandle* prepareVBO( Sprite* sprites, unsigned int scount );


	bool drawToNewGLTexture( GLuint* ahandle, GLuint* nhandle, int width, int height, std::vector< TextureProxy* >& textures );
	bool drawToNewGLTexture( GLuint* ahandle, int width, int height, Sprite* sprites, unsigned int count );
	//bool drawToGLTexture( GLuint ahandle, std::vector< TextureProxy* >& textures );


}


#endif /* TEXTUREARRAY_H_ */
