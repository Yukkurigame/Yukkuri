/*
 * TextureArray.cpp
 *
 *  Created on: 28.05.2012
 *
 */

#include "graphics/render/TextureArray.h"
#include "graphics/render/GLHelpers.h"
#include "graphics/gl_extensions.h"
#include "graphics/Render.h"

#include <algorithm>

#include "hacks.h"


namespace {

	inline bool compareSprites( Sprite* s1, Sprite* s2 )
	{
		if( s1->vertices.rt.z == s2->vertices.rt.z ){
			if( s1->rect.y == s2->rect.y ){
				return ( s1->rect.x > s2->rect.x );
			}
			return ( s1->rect.y > s2->rect.y );
		}
		return ( s1->vertices.rt.z < s2->vertices.rt.z );
	}

}

/*	This function make vbo array from sprite array
 *	c - count of vbos handles to be returned
 *	sprites - vector of sprites
 *	verticles - vbo data array
 *	returns pointer to the first vbo info handler
 */
VBOStructureHandle* TextureArray::prepareVBO( int* c, std::vector< Sprite* >& sprites, VertexV2FT2FC4UI* verticles )
{
	sort(sprites.begin(), sprites.end(), compareSprites);
	int count = 0;
	Sprite* s;
	//VBOHandlesCount = 0;
	VBOStructureHandle* v = NULL;
	VBOStructureHandle* first = NULL;
	//int lastText = -1;
	//memset( verticles, '0', sizeof( VertexV2FT2FC4UI ) * verticlesSize );
	FOREACHIT( sprites ){
		s = *(it);
		if( s == NULL || !s->isVisible() )
			continue;
		/*if( s->atlas != lastText ){
			if( VBOHandlesCount + 1 > VBOHandlesSize ){
				VBOHandlesSize = (VBOHandlesCount + 1) * 2;
				VBOHandles = (VBOStructureHandle*)realloc( VBOHandles, sizeof(VBOStructureHandle) * VBOHandlesSize );
			}
			VBOStructureHandle* sh = &VBOHandles[VBOHandlesCount];
			sh->atlas = lastText = s->atlas;
			sh->start = count;
			if( VBOHandlesCount )
				VBOHandles[VBOHandlesCount - 1].count = count - VBOHandles[VBOHandlesCount - 1].start;
			VBOHandlesCount++;
		}*/

		// TODO: atlas instead of texture
		if( !v ){
			first = v = new VBOStructureHandle(s->atlas, s->shader, count);
		}else if( s->atlas != v->atlas || s->shader != v->shader ){
			v->next = new VBOStructureHandle( s->atlas, s->shader, count );
			v->count = count - v->start;
			v = v->next;
		}

		// TODO: place VertexV2FT2FC4UI to sprite instead of verticles, coordinates and color
		verticles[count++].set( &s->vertices.lb, &s->coordinates.lt, &s->clr );
		verticles[count++].set( &s->vertices.rb, &s->coordinates.rt, &s->clr );
		verticles[count++].set( &s->vertices.rt, &s->coordinates.rb, &s->clr );
		verticles[count++].set( &s->vertices.lt, &s->coordinates.lb, &s->clr );
	}
	//if( VBOHandlesCount )
	//	VBOHandles[VBOHandlesCount - 1].count = count - VBOHandles[VBOHandlesCount - 1].start;
	if( v != NULL)
		v->count = count - v->start;
	(*c) = count;
	return first;
}


/*	This function make vbo array from sprite array
 *	c - count of vbos handles to be returned
 *	sprites - Pointer to array of sprites
 *	scount - count of sprites in array
 *	verticles - vbo data array
 *	returns pointer to the first vbo info handler
 */
VBOStructureHandle* TextureArray::prepareVBO( int* c, Sprite* sprites, unsigned int scount, VertexV2FT2FC4UI* verticles )
{
	int count = 0;
	VBOStructureHandle* v = NULL;
	VBOStructureHandle* first = NULL;
	for( unsigned int i = 0; i < scount; ++i ){
		Sprite& s = sprites[i];
		if( !s.isVisible() )
			continue;
		// TODO: atlas instead of texture
		if( !v ){
			first = v = new VBOStructureHandle( s.atlas, s.shader, count );
		}else if( s.atlas != v->atlas || s.shader != v->shader ){
			v->next = new VBOStructureHandle( s.atlas, s.shader, count );
			v->count = count - v->start;
			v = v->next;
		}

		verticles[count++].set( &s.vertices.lb, &s.coordinates.lt, &s.clr );
		verticles[count++].set( &s.vertices.rb, &s.coordinates.rt, &s.clr );
		verticles[count++].set( &s.vertices.rt, &s.coordinates.rb, &s.clr );
		verticles[count++].set( &s.vertices.lt, &s.coordinates.lb, &s.clr );
	}
	if( v != NULL)
		v->count = count - v->start;
	(*c) = count;
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
			glVertex2f(t->abs.x, t->abs.y);
			//Bottom-right vertex
			glTexCoord2f(x + dx, y);
			glVertex2f(t->abs.x + t->abs.width, t->abs.y);
			//Top-right vertex
			glTexCoord2f(x + dx, y + dy);
			glVertex2f(t->abs.x + t->abs.width, t->abs.y + t->abs.height);
			//Top-left vertex
			glTexCoord2f(x, y + dy);
			glVertex2f(t->abs.x, t->abs.y + t->abs.height);
		}
		glEnd();

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
	GLHelpers::ClearView( );
	return  GLHelpers::UnbindFBO( FBOHandle );
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

	VertexV2FT2FC4UI* vertices = (VertexV2FT2FC4UI*)malloc( sizeof(VertexV2FT2FC4UI) * count * 4 );
	int vboc = 0;
	VBOStructureHandle* vbostructure = prepareVBO( &vboc, sprites, count, vertices );

	glGenBuffers(1, &VBOHandle);
	GLHelpers::DrawVBO( VBOHandle, vboc, vbostructure, vertices );
	glDeleteBuffers( 1, &VBOHandle );

	free( vertices );

	// Reset FBO
	GLHelpers::ClearView( );
	return GLHelpers::UnbindFBO( FBOHandle );
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
