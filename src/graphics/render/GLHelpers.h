/*
 * GLHelpers.h
 *
 *  Created on: 28.05.2012
 *
 */

#ifndef GLHELPERS_H_
#define GLHELPERS_H_

#include "graphics/GraphicsTypes.h"


namespace GLHelpers
{
	// Core functions
	//bool SetUpView( int width, int height, short vflip = 0 );
	//bool ClearView( );

	// Frame buffer object
	bool BindTextureToFBO( GLuint ahandle, GLuint& FBOHandle );
	bool UnbindFBO( GLuint& FBOHandle );
	bool UpdateTexture( GLuint basetex, Texture* copysrc, int posx, int posy );
	bool GenerateMipmap( GLuint ahandle );

	// Pixel buffer object
	bool UpdateGLTextureFromTexture( GLuint basetex, Texture* copysrc, const s2f& offset,
									int tx, int ty, int w, int h );
	void DrawGLTextureToPBO( GLuint tex, GLuint buf, int size, void* buffer );

	// Simple drawing
	void DrawToQuad( GLuint tex, int x, int y, int width, int height );
}


#endif /* GLHELPERS_H_ */
