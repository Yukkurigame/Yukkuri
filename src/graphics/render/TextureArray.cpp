/*
 * TextureArray.cpp
 *
 *  Created on: 28.05.2012
 *
 */

#include "graphics/render/TextureArray.h"
#include "graphics/render/GLHelpers.h"
#include "graphics/GraphicsTypes.h"
#include "graphics/gl_extensions.h"
#include "graphics/Render.h"

#include <algorithm>

#include "hacks.h"


namespace {

	inline bool compareSprites( Sprite* s1, Sprite* s2 )
	{
		s3f* o1 = &s1->brush.vertex_origin;
		s3f* o2 = &s2->brush.vertex_origin;
		if( o1->z == o2->z ){
			if( o1->y == o2->y ){
				return ( o1->x > o2->x );
			}
			return ( o1->y > o2->y );
		}
		return ( o1->z < o2->z );
	}

}


inline void prepare_vbo( Sprite* s, VBOStructureHandle*& v, VBOStructureHandle*& first )
{
	if( s == NULL || !s->isVisible() )
		return;

	if( !v ){
		first = v = new VBOStructureHandle( s->brush.type, s->atlas, s->shader );
	}else if( v->type != prQUADS || s->atlas != v->atlas || s->shader != v->shader ){
		v->next = new VBOStructureHandle( s->brush.type, s->atlas, s->shader );
		v = v->next;
	}

	v->set_indexes( s->brush.point_index, s->brush.points_count );
}


/*	This function make vbo array from sprite array
 *	c - count of vbos handles to be returned
 *	sprites - vector of sprites
 *	verticles - vbo data array
 *	returns pointer to the first vbo info handler
 */
VBOStructureHandle* TextureArray::prepareVBO( std::vector< Sprite* >& sprites )
{
	sort(sprites.begin(), sprites.end(), compareSprites);
	VBOStructureHandle* v = NULL;
	VBOStructureHandle* first = NULL;
	FOREACHIT( sprites ){
		Sprite* s = *(it);
		prepare_vbo( s, v, first );
	}
	return first;
}


/*	This function make vbo array from sprite array
 *	c - count of vbos handles to be returned
 *	sprites - Pointer to array of sprites
 *	scount - count of sprites in array
 *	verticles - vbo data array
 *	returns pointer to the first vbo info handler
 */
VBOStructureHandle* TextureArray::prepareVBO( Sprite* sprites, unsigned int scount )
{
	//int count = 0;
	VBOStructureHandle* v = NULL;
	VBOStructureHandle* first = NULL;
	for( unsigned int i = 0; i < scount; ++i ){
		Sprite* s = &sprites[i];
		prepare_vbo( s, v, first );
	}
	return first;
}



/* This function render textures array into new opengl texture (or clear it).
 * ahandle - pointer on opengl texture, if points to 0 new texture will be generated;
 * width, height - width and height of new texture;
 * textures - array of textures coordinates to draw;
 * returns boolean
 */
bool TextureArray::drawToNewGLTexture( GLuint* ahandle, int width, int height, std::vector< TextureProxy* >& textures )
{
	GLuint FBOHandle;

	// A FBO will be used to draw textures. FBO creation and setup.
	if( !GLHelpers::CreateTexture( ahandle, width, height ) ||
		!GLHelpers::BindTextureToFBO( *ahandle, FBOHandle ) ||
		!GLHelpers::SetUpView( width, height ) )
		return false;

	// Draw textures into atlas.
	glEnable(GL_TEXTURE_2D);
	for ( unsigned int i = 0; i < textures.size(); i++ ){
		TextureProxy* t = textures.at(i);
		if( t->texture ){
			glBindTexture( GL_TEXTURE_2D, t->texture->tex );
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
bool TextureArray::drawToNewGLTexture( GLuint* ahandle, int width, int height, Sprite* sprites, unsigned int count )
{
	GLuint FBOHandle;
	GLuint VBOHandle;

	// A FBO will be used to draw textures. FBO creation and setup.
	if( !GLHelpers::CreateTexture( ahandle, width, height ) ||
		!GLHelpers::BindTextureToFBO( *ahandle, FBOHandle ) ||
		!GLHelpers::SetUpView( width, height, 1 ) )
		return false;

	VBOStructureHandle* vbostructure = prepareVBO( sprites, count );

	glGenBuffers(1, &VBOHandle);
	GLHelpers::DrawVBO( VBOHandle, vbostructure );
	glDeleteBuffers( 1, &VBOHandle );

	/* free( vertices ); */

	// Reset FBO
	return  GLHelpers::ClearView( ) &&
			GLHelpers::UnbindFBO( FBOHandle );
}


/* This function render textures array into opengl texture.
 * ahandle - an opengl texture id, it must be generated previously;
 * textures - an array of textures coordinates to draw;
 * returns boolean
 */
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
