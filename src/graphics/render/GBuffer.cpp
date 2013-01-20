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
#include "graphics/Lighting.h"
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

	const int texture_type = GL_TEXTURE_2D;

	void geometry_pass( );
	void stencil_pass( LightSource* );
	void light_pass_point( LightSource* );
	void light_pass_directional( );
	void final_pass( );

	inline void texture_to_fbo( GLuint tex_id, int int_format, int format, int type, int attach )
	{
		glBindTexture( texture_type, tex_id );
		glTexImage2D( texture_type, 0, int_format, conf.video.windowWidth, conf.video.windowHeight,
						0, format, type, NULL );
		glTexParameterf( texture_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameterf( texture_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, attach, texture_type, tex_id, 0 );
	}
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
		texture_to_fbo( textures[i], GL_RGBA, GL_RGBA, GL_UNSIGNED_INT, GL_COLOR_ATTACHMENT0 + i );
	}

	// Depth texture
	texture_to_fbo( depth_texture, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_DEPTH_ATTACHMENT );
	glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, texture_type, depth_texture, 0 );

	// Result texture
	texture_to_fbo( final_texture, GL_RGBA, GL_RGBA, GL_UNSIGNED_INT, GL_COLOR_ATTACHMENT4 );

	GLenum fbstatus = glCheckFramebufferStatus( GL_FRAMEBUFFER );

	if( fbstatus != GL_FRAMEBUFFER_COMPLETE ){
		Debug::debug( Debug::GRAPHICS, "Error in g-buffer framebuffer initialization.\n");
		return false;
	}

	// restore default FBO
	glBindTexture( texture_type, 0 );
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

	GLHelpers::BindVBO( VBOHandle );
	GLHelpers::FillVBO();

	geometry_pass( );

	glEnable(GL_STENCIL_TEST);

	listElement<LightSource*>* light = LightingManager::first( ltPoint );
	while( light != NULL ){
		stencil_pass( light->data );
		light_pass_point( light->data );
		light = light->next;
	}

	glDisable(GL_STENCIL_TEST);

	light_pass_directional( );


	GLHelpers::UnbindVBO( );

	// Draw to final texture
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
	glBindFramebuffer( GL_READ_FRAMEBUFFER, fbo );
	glReadBuffer( GL_COLOR_ATTACHMENT4 );

	// Blit final texture to screen
	glBlitFramebuffer(0, 0, conf.video.windowWidth, conf.video.windowHeight, 0, 0,
					conf.video.windowWidth, conf.video.windowHeight,
					GL_COLOR_BUFFER_BIT, GL_LINEAR);

}

void GBuffer::geometry_pass( )
{
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo );

	GLenum draw_buffers[3] = { GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2 };

	glDrawBuffersARB( 3, draw_buffers );

	// Only the geometry pass updates the depth buffer
	glDepthMask( GL_TRUE );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );

	VBOStructureHandle* vbos = TextureArray::prepareVBO(
			glpGeometry, RenderManager::GetSpritesArray() );

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	GLHelpers::DrawVBO( vbos );

	glDisable(GL_BLEND);

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


void GBuffer::stencil_pass( LightSource* )
{

}


void GBuffer::light_pass_point( LightSource* )
{
}


void GBuffer::light_pass_directional( )
{
	glDrawBuffer( GL_COLOR_ATTACHMENT4 );

	for (unsigned int i = 0 ; i < gbufLast; i++) {
		glActiveTexture( GL_TEXTURE2 + i );
		glBindTexture( texture_type, textures[i] );
	}

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	VBOStructureHandle* vbos = TextureArray::prepareVBO(
			glpDirLight, RenderManager::GetSpritesArray() );
	GLHelpers::DrawVBO( vbos );

	VBOStructureHandle* temp;
	while( vbos != NULL ){
		temp = vbos;
		vbos = vbos->next;
		delete temp;
	}

	glDisable(GL_BLEND);
}


void GBuffer::final_pass( )
{
}
