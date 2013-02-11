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
	// Frame buffer object
	bool BindTextureToFBO( GLuint ahandle, GLuint& FBOHandle );
	bool UnbindFBO( GLuint& FBOHandle );
	bool UpdateTexture( GLuint basetex, Texture* copysrc, int posx, int posy );
	bool GenerateMipmap( GLuint ahandle );

	// Simple drawing
	void DrawToQuad( GLuint tex, int x, int y, int width, int height );
}


#endif /* GLHELPERS_H_ */
