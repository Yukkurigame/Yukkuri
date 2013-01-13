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
	// Create the FBO
	glGenFramebuffersEXT( 1, &fbo );
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo );

	// Create the gbuffer textures
	glGenTextures( gbufLast, textures );
	glGenTextures( 1, &depth_texture );
	glGenTextures( 1, &final_texture );

	for( unsigned int i = 0 ; i < gbufLast; i++ ){
		glBindTexture( GL_TEXTURE_2D, textures[i] );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB32F_ARB, conf.windowWidth, conf.windowHeight, 0, GL_RGB, GL_FLOAT, NULL );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, textures[i], 0 );
	}

	// depth
	glBindTexture( GL_TEXTURE_2D, depth_texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, conf.windowWidth, conf.windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL );
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depth_texture, 0 );

	// final
	glBindTexture( GL_TEXTURE_2D, final_texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, conf.windowWidth, conf.windowHeight, 0, GL_RGB, GL_FLOAT, NULL );
	glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT4_EXT, GL_TEXTURE_2D, final_texture, 0 );

	GLenum fbstatus = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );

	if( fbstatus != GL_FRAMEBUFFER_COMPLETE_EXT ){
		Debug::debug( Debug::GRAPHICS, "Error in g-buffer framebuffer initialization.\n");
		return false;
	}

	// restore default FBO
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

	return true;
}

void GBuffer::clean( )
{
	if( fbo )
		glDeleteFramebuffersEXT( 1, &fbo );

	if( textures[0] )
		glDeleteTextures( gbufLast, textures );

	if( depth_texture )
		glDeleteTextures( 1, &depth_texture );

	if( final_texture != 0 )
		glDeleteTextures( 1, &final_texture );
}


void GBuffer::render()
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo );
	glDrawBuffer( GL_COLOR_ATTACHMENT4_EXT );
	glClear( GL_COLOR_BUFFER_BIT );

	geometry_pass( );


}

void GBuffer::geometry_pass( )
{
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo );

	GLenum draw_buffers[3] = { GL_COLOR_ATTACHMENT0_EXT,
			GL_COLOR_ATTACHMENT1_EXT,
			GL_COLOR_ATTACHMENT2_EXT };

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
