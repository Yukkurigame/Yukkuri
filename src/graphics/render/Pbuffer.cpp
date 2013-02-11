/*
 * Pbuffer.cpp
 *
 *  Created on: 08.02.2013
 */

#include "graphics/render/PBuffer.h"


/* This function copy texture part to base texture using PBO.
 * basetex - opengl id of base texture.
 * copysrc - texture structure with copy source.
 * offset - offset in base texture.
 * w, h - width and height of base texture
 * tx, ty - offset in copy source texture
 * returns bool
 */
bool PBuffer::copy( GLuint basetex, Texture* copysrc, const s2f& offset, int tx, int ty, int w, int h )
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
	glBindBuffer( GL_PIXEL_UNPACK_BUFFER, PBOHandle );
	glBufferData( GL_PIXEL_UNPACK_BUFFER, size, NULL, GL_STREAM_DRAW );

	// Map the buffer buffer
	void* PBOMemory = glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY );
	if( PBOMemory == NULL )
		Debug::debug( Debug::GRAPHICS, "PBO mapping failed.\n" );

	draw( copysrc->tex, PBOHandle, size, PBOMemory );

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
void PBuffer::draw( GLuint tex, GLuint buf, int size, void* buffer )
{
	// Bind buffer for packing
	glBindBuffer( GL_PIXEL_PACK_BUFFER, buf );
	// Not sure if this needed
	glActiveTexture( GL_TEXTURE0 );
	// Bind texture and write its data to buffer
	Textures::bind(tex, 0, GL_TEXTURE_2D );
	glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
	// Unbind buffer and texture
	glBindBuffer( GL_PIXEL_PACK_BUFFER, 0 );
	Textures::unbind( tex, GL_TEXTURE_2D );
}
