/*
 * GBuffer.cpp
 *
 *  Created on: 11.01.2013
 *
 */

#include "graphics/render/GBuffer.h"
#include "graphics/render/VBuffer.h"
#include "graphics/render/Textures.h"
#include "graphics/utils/gl_uniforms.h"
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

	s2f window_size;

	int in_ScreenSize = -1;

	const int texture_type = GL_TEXTURE_2D;

	void geometry_pass( list<VBOStructureHandle*>* handler  );
	void stencil_pass( LightSource* );
	void light_pass_point( LightSource* );
	void light_pass_directional( list<VBOStructureHandle*>* handler );
	void final_pass( );

	inline void texture_to_fbo( GLuint tex_id, int int_format, int format, int type, int attach )
	{
		Textures::bind( tex_id, 0, texture_type );
		glTexImage2D( texture_type, 0, int_format, window_size.x, window_size.y,
						0, format, type, NULL );
		glTexParameterf( texture_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameterf( texture_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, attach, texture_type, tex_id, 0 );
	}

	Sprite* combinator;
	list< VBOStructureHandle* > combinator_handler;
}



bool GBuffer::init()
{
	window_size.x = conf.video.windowWidth;
	window_size.y = conf.video.windowHeight;

	combinator = new Sprite( );
	combinator->resize( window_size.x, window_size.y );
	combinator->setFixed();

	in_ScreenSize = UniformsManager::register_uniform( "in_ScreenSize", GL_FLOAT_VEC2 );
	UniformsManager::pass_data( in_ScreenSize, &window_size.x );

	// Create the VBO
	glGenBuffers(1, &VBOHandle);

	// Create the FBO
	glGenFramebuffers( 1, &fbo );
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo );

	// Create the gbuffer textures
	glGenTextures( gbufLast, textures );
	glGenTextures( 1, &depth_texture );
	glGenTextures( 1, &final_texture );

	// FIXME: WTF is all this shit?
	UINT indexes[gbufLast];
	const char* names[gbufLast] = { "in_gNoLightMap", "in_gPositionMap", "in_gNormalMap", "in_gColorMap" };
	for( UINT i = 0 ; i < gbufLast; i++ ){
		texture_to_fbo( textures[i], GL_RGBA, GL_RGBA, GL_UNSIGNED_INT, GL_COLOR_ATTACHMENT0 + i );
		indexes[i] = UniformsManager::register_uniform( names[i], GL_SAMPLER_2D );
	}

	const GLMaterial* mat = GLMaterialManager::get_pointer( combinator->material );
	UniformHandlers* uniforms = mat->uniforms[glpDirLight];
	for( UINT i = 0, index = 0; i < uniforms->count; ++i ){
		UniformHandler* h = &(uniforms->handlers[i]);
		if( h->type != GL_SAMPLER_2D || index >= gbufLast )
			continue;
		for( UINT j = 0 ; j < gbufLast; ++j ){
			if( h->index == indexes[j] )
				combinator->textures.push_back( textures[j] );
		}
	}

	// Prepare combinator to rendering
	VBuffer::prepare_handler( combinator, &combinator_handler );


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
	Textures::unbind( );
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

	VBuffer::setup( VBOHandle );

	list< VBOStructureHandle* > vbos;
	VBuffer::prepare_handler( RenderManager::GetSpritesArray(), &vbos );
	geometry_pass( &vbos );
	VBuffer::free_handler( &vbos );

	glEnable(GL_STENCIL_TEST);

	listElement<LightSource*>* light = LightingManager::first( ltPoint );
	while( light != NULL ){
		stencil_pass( light->data );
		light_pass_point( light->data );
		light = light->next;
	}

	glDisable(GL_STENCIL_TEST);

	light_pass_directional( &combinator_handler );

	VBuffer::unbind( );


	// Draw to final texture
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
	glBindFramebuffer( GL_READ_FRAMEBUFFER, fbo );
	glReadBuffer( GL_COLOR_ATTACHMENT4 );

	// Blit final texture to screen
	glBlitFramebuffer(0, 0, conf.video.windowWidth, conf.video.windowHeight, 0, 0,
					conf.video.windowWidth, conf.video.windowHeight,
					GL_COLOR_BUFFER_BIT, GL_LINEAR);

}

void GBuffer::geometry_pass( list<VBOStructureHandle*>* handler )
{
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, fbo );

	GLenum draw_buffers[4] = { GL_COLOR_ATTACHMENT0,
			GL_COLOR_ATTACHMENT1,
			GL_COLOR_ATTACHMENT2,
			GL_COLOR_ATTACHMENT3 };

	glDrawBuffersARB( 4, draw_buffers );

	// Only the geometry pass updates the depth buffer
	glDepthMask( GL_TRUE );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	VBuffer::draw( glpGeometry, handler );

	glDisable(GL_BLEND);

	// When we get here the depth buffer is already populated and the stencil pass
	// depends on it, but it does not write to it.
	glDepthMask( GL_FALSE );

}


inline float point_light_area( const s3fc& color, float intensity )
{
	float c = fmax(fmax(color.r, color.g), color.b) * intensity;
	return (8.0f * sqrtf(c) + 1.0f);
}

void GBuffer::stencil_pass( LightSource* )
{
	glDrawBuffer(GL_NONE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClear(GL_STENCIL_BUFFER_BIT);
	// We need the stencil test to be enabled but we want it
	// to succeed always. Only the depth test matters.
	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR, GL_KEEP);


}


void GBuffer::light_pass_point( LightSource* )
{
}


void GBuffer::light_pass_directional( list<VBOStructureHandle*>* handler )
{
	glDrawBuffer( GL_COLOR_ATTACHMENT4 );

	glDisable(GL_DEPTH_TEST);

	//glEnable(GL_BLEND);
	//glBlendEquation(GL_FUNC_ADD);
	//glBlendFunc(GL_ONE, GL_ONE);

	VBuffer::draw( glpDirLight, handler );

	//glDisable(GL_BLEND);

}


void GBuffer::final_pass( )
{
}
