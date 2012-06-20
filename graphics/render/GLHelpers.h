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
	bool CreateTexture( GLuint* ahandle, int width, int height );
	bool SetUpView( int width, int height, short vflip = 0 );
	bool ClearView( );

	// Vertex buffer object
	void DrawVBO( int vboc, VBOStructureHandle* vbostructure, VertexV2FT2FC4UI* vertices );

	// Frame buffer object
	bool BindTextureToFBO( GLuint ahandle, GLuint& FBOHandle );
	bool UnbindFBO( GLuint& FBOHandle );
	bool UpdateTexture( GLuint basetex, Texture* copysrc, int posx, int posy );

	// Pixel buffer object
	bool UpdateGLTextureFromTexture( GLuint basetex, Texture* copysrc, const s2f& offset,
									int tx, int ty, int w, int h );
	void DrawGLTextureToPBO( GLuint tex, GLuint buf, int size, void* buffer );

	// Simple drawing
	void DrawToQuad( GLuint tex, int x, int y, int width, int height );
}


#endif /* GLHELPERS_H_ */
