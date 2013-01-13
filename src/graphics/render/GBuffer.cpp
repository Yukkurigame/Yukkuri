/*
 * GBuffer.cpp
 *
 *  Created on: 11.01.2013
 *
 */

#include "graphics/render/GBuffer.h"
#include "graphics/gl_extensions.h"
#include "graphics/utils/gl_shader.h"
#include "graphics/render/TextureArray.h"
#include "graphics/render/GLHelpers.h"
#include "graphics/Render.h"

#include "config.h"
#include "debug.h"


extern MainConfig conf;


namespace GBuffer
{
	GLuint fbo;
	GLuint textures[gbufLast];
	GLuint depth_texture;
	GLuint final_texture;

	GLuint VBOHandle;
}



bool GBuffer::init()
{
	// Create the VBO
	glGenBuffers(1, &VBOHandle);

	// Create the FBO
	glGenFramebuffers( 1, &fbo );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo );

	// Create the gbuffer textures
	glGenTextures( gbufLast, textures );
	glGenTextures( 1, &depth_texture );
	glGenTextures( 1, &final_texture );

	for( unsigned int i = 0 ; i < gbufLast; i++ ){
		glBindTexture( GL_TEXTURE_2D, textures[i] );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32F, conf.windowWidth, conf.windowHeight, 0, GL_RGB, GL_FLOAT, NULL );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, textures[i], 0 );
	}

	// Depth texture
	glBindTexture( GL_TEXTURE_2D, depth_texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH32F_STENCIL8, conf.windowWidth, conf.windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
	glFramebufferTexture2DEXT( GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0 );

	// Result texture
	glBindTexture( GL_TEXTURE_2D, final_texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, conf.windowWidth, conf.windowHeight, 0, GL_RGB, GL_FLOAT, NULL );
	glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, final_texture, 0 );

	GLenum fbstatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );

	if( fbstatus != GL_FRAMEBUFFER_COMPLETE ){
		Debug::debug( Debug::GRAPHICS, "Error in g-buffer framebuffer initialization.\n");
		return false;
	}

	// restore default FBO
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );

	return true;
}


void GBuffer::clean( )
{
	if( VBOHandle )
		glDeleteBuffers( 1, &VBOHandle );

	if( fbo )
		glDeleteFramebuffers( 1, &fbo );

	if( textures[0] )
		glDeleteTextures( gbufLast, textures );

	if( depth_texture )
		glDeleteTextures( 1, &depth_texture );

	if( final_texture != 0 )
		glDeleteTextures( 1, &final_texture );

}


void GBuffer::render()
{
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo );
	glDrawBuffer( GL_COLOR_ATTACHMENT4 );
	glClear( GL_COLOR_BUFFER_BIT );

	geometry_pass( );

	// Draw to final texture
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
    glBindFramebuffer( GL_READ_FRAMEBUFFER, fbo );
    glReadBuffer( GL_COLOR_ATTACHMENT4 );

    // Blit final texture to screen
    glBlitFramebuffer(0, 0, conf.windowWidth, conf.windowHeight, 0, 0,
                      conf.windowWidth, conf.windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

}

void GBuffer::geometry_pass( )
{
	glBindFramebufferEXT( GL_DRAW_FRAMEBUFFER, fbo );

	GLenum draw_buffers[3] = { GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2 };

	glDrawBuffers( 3, draw_buffers );

	// Only the geometry pass updates the depth buffer
	glDepthMask( GL_TRUE );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );

	VBOStructureHandle* vbos = TextureArray::prepareVBO(
			glpGeometry, RenderManager::GetSpritesArray() );

	GLHelpers::DrawVBO( VBOHandle, vbos );

	VBOStructureHandle* temp;
	while( vbos != NULL ){
		temp = vbos;
		vbos = vbos->next;
		delete temp;
	}

	// When we get here the depth buffer is already populated and the stencil pass
	// depends on it, but it does not write to it.
	glDepthMask( GL_FALSE );

}

void GBuffer::stencil_pass( )
{
}

void GBuffer::light_pass( )
{
}

void GBuffer::final_pass( )
{
}
