/*
 * TextureArray.cpp
 *
 *  Created on: 28.05.2012
 *
 */

#include "graphics/render/TextureArray.h"
#include "graphics/render/GLHelpers.h"
#include "graphics/render/VBuffer.h"
#include "graphics/utils/gl_shader.h"
#include "graphics/Camera.h"
#include "graphics/GraphicsTypes.h"
#include "graphics/gl_extensions.h"
#include "graphics/Render.h"

#include <algorithm>

#include "debug.h"
#include "hacks.h"


inline void draw_proxy_quad( TextureProxy* t, bool normal = false )
{
	if( t && t->texture ){
		if( normal && !t->texture->normals )
			return;
		glBindTexture( GL_TEXTURE_2D, normal ? t->texture->normals : t->texture->tex );
		glBegin(GL_QUADS);
		{
			// Position in original texture
			float x = static_cast<float>(t->offset.x) / static_cast<float>(t->texture->w);
			float y = static_cast<float>(t->offset.y) / static_cast<float>(t->texture->h);
			float dx = static_cast<float>(t->abs.width) / static_cast<float>(t->texture->w);
			float dy = static_cast<float>(t->abs.height) / static_cast<float>(t->texture->h);
			//Bottom-left vertex
			glTexCoord2f(x, y);
			glVertex2i(t->abs.x, t->abs.y);
			//Bottom-right vertex
			glTexCoord2f(x + dx, y);
			glVertex2i(t->abs.x + t->abs.width, t->abs.y);
			//Top-right vertex
			glTexCoord2f(x + dx, y + dy);
			glVertex2i(t->abs.x + t->abs.width, t->abs.y + t->abs.height);
			//Top-left vertex
			glTexCoord2f(x, y + dy);
			glVertex2i(t->abs.x, t->abs.y + t->abs.height);
		}
		glEnd();
	}
}


/* This function render textures array into new opengl texture (or clear it).
 * ahandle - pointer on opengl texture, if points to 0 new texture will be generated;
 * width, height - width and height of new texture;
 * textures - array of textures coordinates to draw;
 * returns boolean
 */
bool TextureArray::drawToNewGLTexture( GLuint* ahandle, GLuint* nhandle, int width, int height, std::vector< TextureProxy* >& textures )
{
	GLuint FBOHandle = 0;

	// A FBO will be used to draw textures. FBO creation and setup.
	if( !GLHelpers::CreateTexture( ahandle, width, height ) ||
		!GLHelpers::BindTextureToFBO( *ahandle, FBOHandle ) ||
		!GLHelpers::SetUpView( width, height ) )
		return false;

	// Draw textures into atlas.
	glEnable(GL_TEXTURE_2D);
	for ( unsigned int i = 0; i < textures.size(); i++ ){
		TextureProxy* t = textures.at(i);
		draw_proxy_quad( t );

		if( nhandle && !*nhandle && t->texture && t->texture->normals ){
			if( !GLHelpers::CreateTexture( nhandle, width, height ) )
				Debug::debug( Debug::GRAPHICS, "Cannot create normal map for textures.\n" );
		}

#ifdef DEBUG_DRAW_ATLAS_RECTANGLES
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_LINE_LOOP);
			glVertex2f(t->abs.x, t->abs.y);
			glVertex2f(t->abs.x + t->abs.width, t->abs.y);
			glVertex2f(t->abs.x + t->abs.width, t->abs.y + t->abs.height);
			glVertex2f(t->abs.x, t->abs.y + t->abs.height);
		glEnd();
		glEnable(GL_TEXTURE_2D);
#endif
	}

	if( nhandle && *nhandle ){
		GLHelpers::BindTextureToFBO( *nhandle, FBOHandle );
		for ( unsigned int i = 0; i < textures.size(); i++ ){
			TextureProxy* t = textures.at(i);
			draw_proxy_quad( t, true );
		}
	}


	// Reset FBO
	return  GLHelpers::ClearView( ) &&
			GLHelpers::UnbindFBO( FBOHandle );
}

/* This function render sprites array into new opengl texture (or clear it).
 * ahandle - pointer on opengl texture, if points to 0 new texture will be generated;
 * width, height - width and height of new texture;
 * sprites - array of sprites to draw;
 * returns boolean
 */
bool TextureArray::drawToNewGLTexture( GLuint* ahandle, int width, int height, list< Sprite* >* sprites )
{
	GLuint FBOHandle = 0;
	GLuint VBOHandle = 0;

	// A FBO will be used to draw textures. FBO creation and setup.
	if( !GLHelpers::CreateTexture( ahandle, width, height ) ||
		!GLHelpers::BindTextureToFBO( *ahandle, FBOHandle ) )
		return false;

	rect2f new_camera(0, height, width, -height);
	Camera::push_state( &new_camera );
	Camera::Update();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glGenBuffers( 1, &VBOHandle );

	VBOStructureHandle* vbostructure = VBuffer::prepare_handler( glpSimple, sprites );
	VBuffer::setup( VBOHandle );
	VBuffer::draw( vbostructure );
	VBuffer::unbind( );
	VBuffer::free_handler( &vbostructure );

	glDeleteBuffers( 1, &VBOHandle );
	glDisable(GL_BLEND);

	Camera::pop_state();

	//GLHelpers::ClearView( );

	// Reset FBO
	return  GLHelpers::UnbindFBO( FBOHandle );
}



/* This function render textures array into opengl texture.
 * ahandle - an opengl texture id, it must be generated previously;
 * textures - an array of textures coordinates to draw;
 * returns boolean

bool TextureArray::drawToGLTexture( GLuint ahandle, std::vector< TextureProxy* >& textures )
{

	// Отрисовка текстур в атлас.
	glEnable(GL_TEXTURE_2D);
	for( unsigned int i = 0; i < textures.size(); i++ ){
		//TextureProxy* t = textures->at(i);
		//UpdateGLTextureFromTexture( ahandle, t->texture, t->offset, t->abs.x, t->abs.y,
		//							t->abs.width, t->abs.height );
	}
	glDisable(GL_TEXTURE_2D);

	return true;
}
 */
