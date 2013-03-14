/*
 * GLTextures.cpp
 *
 *  Created on: 17.07.2012
 *
 */


#include "graphics/render/GLTextures.h"
#include "graphics/render/GLHelpers.h"
#include "graphics/render/VBuffer.h"
#include "graphics/render/Textures.h"
#include "graphics/utils/Image.h"
#include "graphics/Camera.h"

#include <map>

#include "hacks.h"
#include "debug.h"


namespace GLTextures {

	std::map < std::string, Texture* > texturesCache;

}


void GLTextures::add( std::string name, Texture* texture )
{
	Texture* cached;
	if( !texture )
		return;
	cached = get( name );
	if( cached == texture )
		return;
	GLTextures::free( cached );
	texturesCache[name] = texture;
}


void GLTextures::clean( )
{
	FOREACHIT1( texturesCache ){
		GLTextures::free( it->second );
		it->second = NULL;
	}
	texturesCache.clear();
}


void GLTextures::free( Texture* tex )
{
	if( tex && tex->tex ){
		glDeleteTextures( 1, &tex->tex );
		glDeleteTextures( 1, &tex->normals );
		delete tex;
		tex = NULL;
	}
}


Texture* GLTextures::get( std::string name )
{
	if( name == "" )
		return NULL;

	std::map < std::string, Texture* >::iterator it;
	it = texturesCache.find(name);
	if( it != texturesCache.end() ){
		return it->second;
	}
	return NULL;
}


Texture* GLTextures::load( std::string name )
{
	Texture* tex = get( name );

	if( tex != NULL )
		return tex;

	int sw, sh, nw, nh;
	unsigned int surface = Image::load( name.c_str(), &sw, &sh );
	unsigned int normals = Image::load( ("normals/" + name).c_str(), &nw, &nh, false );

	if( !surface ){
		Debug::debug( Debug::GRAPHICS, "Image %s not loaded.\n", name.c_str() );
		return NULL;
	}

	tex = create( name, surface, sw, sh );

	if( normals ){
		Debug::debug( Debug::GRAPHICS, "Normals %s loaded.\n", name.c_str() );
		tex->normals = normals;
	}
	return tex;
}



Texture* GLTextures::create( std::string name, GLuint texture, int width, int height )
{
	Texture* tex = new Texture();
	tex->normals = 0;
	tex->tex = texture;
	tex->w = width;
	tex->h = height;
	add( name, tex );
	return tex;
}



/*	This function creates new opengl texture. If texture is exists it will be cleared.
 *	ahandle - pointer to texture id
 *	width, height - texture size
 *	returns boolean
 */
bool GLTextures::generate( GLuint* ahandle, GLenum target, GLint internalformat,
		GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
	// Texture generation and setup.
	if( ahandle == NULL ){
		Debug::debug( Debug::GRAPHICS, "Bad texture for generation.\n" );
		return false;
	}
	if( *ahandle == 0 )
		glGenTextures(1, ahandle);
	Textures::bind( *ahandle, 0, target );
	glTexImage2D( target, 0, internalformat, width, height, 0, format, type, pixels );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( target, GL_TEXTURE_WRAP_T, GL_CLAMP );
	Textures::unbind( *ahandle, target );
	return true;
}






/* This function render sprites array into new opengl texture (or clear it).
 * ahandle - pointer on opengl texture, if points to 0 new texture will be generated;
 * width, height - width and height of new texture;
 * sprites - array of sprites to draw;
 * returns boolean
 */
bool GLTextures::draw( GLuint* ahandle, int width, int height, list< Sprite* >* sprites, bool invert )
{
	GLuint FBOHandle = 0;
	//GLuint VBOHandle = 0;

	// A FBO will be used to draw textures. FBO creation and setup.
	if( !generate( ahandle, width, height ) ||
		!GLHelpers::BindTextureToFBO( *ahandle, FBOHandle ) )
		return false;

	glClear( GL_COLOR_BUFFER_BIT );

	rect2f new_camera;
	if( invert )
		new_camera = rect2f(0, height, width, -height);
	else
		new_camera = rect2f(0, 0, width, height);
	Camera::push_state( &new_camera );
	Camera::update();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	list< VBOStructureHandle* > vbostructure;
	VBuffer::prepare_handler( sprites, &vbostructure );
	VBuffer::draw( glpSimple, &vbostructure );
	VBuffer::free_handler( &vbostructure );

	glDisable(GL_BLEND);

	Camera::pop_state();

	//GLHelpers::ClearView( );

	// Reset FBO
	return  GLHelpers::UnbindFBO( FBOHandle );
}
