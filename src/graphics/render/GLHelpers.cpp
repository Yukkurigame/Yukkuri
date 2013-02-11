/*
 * GLHelpers.cpp
 *
 *  Created on: 28.05.2012
 *
 */

#include "graphics/render/GLHelpers.h"
#include "graphics/render/Textures.h"
#include "debug.h"


/* This function creates new FBO and bind texture to it
 * ahandle - texture id. If texture is 0 it will be generated
 * FBOHandle - FBO id
 * width, height - size of texture to be drawn
 * returns bool
 */
bool GLHelpers::BindTextureToFBO( GLuint ahandle, GLuint& FBOHandle )
{
	if( !FBOHandle )
		glGenFramebuffersEXT(1, &FBOHandle);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOHandle);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, ahandle, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if( status != GL_FRAMEBUFFER_COMPLETE_EXT ){
		Debug::debug( Debug::GRAPHICS, "Your framebuffer is broken. Use top NV to play this Crusis.\n" );
		return false;
	}

	return true;
}



/* This function restore matrices and frees FBO object
 * FBOHandle - FBO id
 * returns bool
 */
bool GLHelpers::UnbindFBO( GLuint& FBOHandle )
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	Textures::unbind( );
	//glBindTexture(GL_TEXTURE_2D, 0);
	// Возвращаем как было.
	glDisable(GL_TEXTURE_2D);
	glDeleteFramebuffersEXT(1, &FBOHandle);
	FBOHandle = 0;
	return true;
}


/*	This function updates texture region from another texture using FBO
 *	basetex - target texture
 *	copystc - source texture with its size
 *	posx, posy - update position
 * 	returns bool
 */
bool GLHelpers::UpdateTexture( GLuint basetex, Texture* copysrc, int posx, int posy )
{
	GLuint FBOHandle = 0;
	if( !BindTextureToFBO( copysrc->tex, FBOHandle ) )
		return false;

	Textures::bind( basetex, 0, GL_TEXTURE_2D );
	glCopyTexSubImage2D( GL_TEXTURE_2D, 0, posx, posy, 0, 0, copysrc->w, copysrc->h );
	Textures::unbind( basetex, GL_TEXTURE_2D );

	return GLHelpers::UnbindFBO( FBOHandle );
}


/*  This function generates mipmaps to textures.
 *  ahandle - texture id
 *  returns bool
 *
 *  Generates very blured textures. Use only for consoles.
 */
bool GLHelpers::GenerateMipmap( GLuint ahandle )
{
	Textures::bind( ahandle, 0, GL_TEXTURE_2D );
	glGenerateMipmapEXT( GL_TEXTURE_2D );
	Textures::unbind( ahandle, GL_TEXTURE_2D );
	return true;
}


/* This function draws opengl texture to quad
 * tex - opengl texture id
 * x, y - position to draw
 * width, height - size of texture
 */
void GLHelpers::DrawToQuad( GLuint tex, int x, int y, int width, int height )
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, tex );
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0, 0.0);
		glVertex2i(x, y);
		glTexCoord2f(1, 0);
		glVertex2i(x + width, y);
		glTexCoord2f(1, 1);
		glVertex2i(x + width, y + height);
		glTexCoord2f(0, 1);
		glVertex2i(x, y + height);
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

#ifdef DEBUG_DRAW_RECTANGLES
	glBegin(GL_LINE_LOOP);
	{
		glVertex2f(x, y);
		glVertex2f(x + width, y);
		glVertex2f(x + width, y + height);
		glVertex2f(x, y + height);
	}
	glEnd();
#endif

}
