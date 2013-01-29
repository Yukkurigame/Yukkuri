/*
 * GLHelpers.cpp
 *
 *  Created on: 28.05.2012
 *
 */

#include "graphics/render/GLHelpers.h"
#include "graphics/utils/VBOArray.h"
#include "graphics/render/Textures.h"
#include "graphics/utils/gl_shader.h"
#include "graphics/gl_extensions.h"

#include "debug.h"


/*	This function setups view for drawing
 * 	width, height - view size
 * 	returns true

bool GLHelpers::SetUpView( int width, int height, short vflip )
{
	glPushAttrib(GL_COLOR_BUFFER_BIT); // ClearColor saving
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Setup viewport and matrices for FBO.
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	if( vflip )
		glOrtho(0, width, height, 0, -2.0, 2.0);
	else
		glOrtho(0, width, 0, height, -2.0, 2.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Clear color
	glClear(GL_COLOR_BUFFER_BIT);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	return true;
}
 */

/*	This function clears view
 * 	returns true

bool GLHelpers::ClearView( )
{
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	//viewport
	glPopAttrib();
	//color
	// OpenGL shows error here, but stack overflows without it
	glPopAttrib();
	return true;
}
 */


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


/* This function copy texture part to base texture using PBO.
 * basetex - opengl id of base texture.
 * copysrc - texture structure with copy source.
 * offset - offset in base texture.
 * w, h - width and height of base texture
 * tx, ty - offset in copy source texture
 * returns bool
 */
bool GLHelpers::UpdateGLTextureFromTexture(
		GLuint basetex, Texture* copysrc, const s2f& offset, int tx, int ty, int w, int h )
{
	if( !basetex ){
		Debug::debug( Debug::GRAPHICS, "Attempt to redraw null texture." );
		return false;
	}

	// Bytes per pixel
	int bpp = 4;

	int size = copysrc->w * copysrc->h * bpp;

	// Create pixel buffer
	GLuint PBOHandle;
	glGenBuffers( 1, &PBOHandle );

	// Bind buffer and fil it with 0
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, PBOHandle );
	glBufferData( GL_PIXEL_UNPACK_BUFFER_ARB, size, NULL, GL_STREAM_DRAW );

	// Map the buffer buffer
	void* PBOMemory = glMapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY );
	if( PBOMemory == NULL )
		Debug::debug( Debug::GRAPHICS, "PBO mapping failed.\n" );

	DrawGLTextureToPBO( copysrc->tex, PBOHandle, size, PBOMemory );

	// Unmap buffer
	glUnmapBuffer( GL_PIXEL_UNPACK_BUFFER_ARB );

	// width * rows * bpp +  x offset cols * bpp
	int bufferofset = (ty * copysrc->w + tx ) * bpp;

	// Update subimage form pixbuffer
	Textures::bind( basetex, 0, GL_TEXTURE_2D );
	glTexSubImage2D( GL_TEXTURE_2D, 0, (GLint)offset.x, (GLint)offset.y, w, h, GL_RGBA,
					GL_UNSIGNED_BYTE, BUFFER_OFFSET(bufferofset) );

	// Unbind buffer
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );


	return true;
}



// You need to enable GL_TEXTURE_2D probably
/* This function draws opengl texture to PBO
 * tex - opengl texture id
 * buf - PBO id
 * size - PBO memory size
 * buffer - pointer to mapped PBO memory
 */
void GLHelpers::DrawGLTextureToPBO( GLuint tex, GLuint buf, int size, void* buffer )
{
	// Bind buffer for packing
	glBindBuffer( GL_PIXEL_PACK_BUFFER_ARB, buf );
	// Not sure if this needed
	glActiveTexture( GL_TEXTURE0 );
	// Bind texture and write its data to buffer
	Textures::bind(tex, 0, GL_TEXTURE_2D );
	glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
	// Unbind buffer and texture
	glBindBuffer( GL_PIXEL_PACK_BUFFER_ARB, 0 );
	Textures::unbind( tex, GL_TEXTURE_2D );
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
