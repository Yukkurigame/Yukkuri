/*
 * Texture.cpp
 *
 *  Created on: 28.05.2012
 *
 * Texture methods of render
 *
 */

#include "graphics/render/Atlas.h"
#include "graphics/render/ElasticBox.h"
#include "graphics/render/TextureArray.h"
#include "graphics/render/GLTextures.h"
#include "graphics/Render.h"

#include "scripts/LuaConfig.h"

#include <algorithm>

#include "safestring.h"
#include "debug.h"
#include "hacks.h"


list< TextureProxy* > internalTextures;

namespace {



	int minAtlasSize;
	int maxAtlasSize;
}

void TextureAtlas::init( )
{
	minAtlasSize = 64;
	glGetIntegerv( GL_MAX_TEXTURE_SIZE, &maxAtlasSize );
}

void TextureAtlas::clean( )
{
	clear_vector( &internalTextures );
}


int TextureAtlas::getAtlasMax( )
{
	return maxAtlasSize;
}


void TextureAtlas::addTexture( std::string name ){
	LuaConfig* lc = new LuaConfig;
	TextureProxy* t = new TextureProxy();
	lc->pushSubconfig( name, "sprite" );
	lc->LuaMain::get( -1, *t );

	/*lc->getValue("id", name, config, t->id);
	lc->getValue("image", name, config, t->image);
	lc->getValue("width", name, config, t->abs.width);
	lc->getValue("height", name, config, t->abs.height);
	lc->getValue("rows", name, config, t->rows);
	lc->getValue("columns", name, config, t->cols);
	lc->getValue("offsetx", name, config, t->offset.x);
	lc->getValue("offsety", name, config, t->offset.y);
	*/

	t->texture = GLTextures::load( t->image );

	internalTextures.push_back(t);

	delete lc;
}


void TextureAtlas::addTexture( std::string id, Texture* tex, int width, int height, int cols, int rows, int ax, int ay ){
	TextureProxy* t = new TextureProxy();
	t->id = id;
	t->abs.width = width;
	t->abs.height = height;
	t->cols = (cols ? cols : 1);
	t->rows = (rows ? rows : 1);
	t->offset.x = t->offset.y = 0;
	t->abs.x = ax;
	t->abs.y = ay;
	t->texture = tex;
	internalTextures.push_back(t);
}


bool TextureAtlas::buildMap( int& width, int& height )
{
	ElasticBox box = ElasticBox( minAtlasSize, maxAtlasSize );

	// Push all textures in ElasticBox packer
	listElement< TextureProxy* >* t = internalTextures.head;
	while( t != NULL ){
		TextureProxy* tp = t->data;
		if ( ! box.InsertItem( &(tp->abs.x), &(tp->abs.y), tp->abs.width, tp->abs.height ) )
			return false;
		t = t->next;
	}

	width = box.Width;
	height = box.Height;

	return buildRelativeMap( (float)box.Width, (float)box.Height );
}


bool TextureAtlas::buildRelativeMap( float width, float height ){

	float texelW = 1.0f;
	float texelH = 1.0f;

	texelW = texelW / width;
	texelH = texelH / height;

	// Build relative map
	listElement< TextureProxy* >* t = internalTextures.head;
	while( t != NULL ){
		TextureProxy* tex = t->data;
		tex->atlas.x = static_cast<float>(tex->abs.x) * texelW;
		tex->atlas.y = static_cast<float>(tex->abs.y) * texelH;
		tex->atlas.width = static_cast<float>(tex->abs.width) * texelW;
		tex->atlas.height = static_cast<float>(tex->abs.height) * texelH;
		t = t->next;
	}

	return true;
}


bool TextureAtlas::build( GLuint* ahandle, GLuint* nhandle, int width, int height )
{
	return TextureArray::drawToNewGLTexture( ahandle, nhandle, width, height, internalTextures );
}



inline bool compareTextureProxies( TextureProxy* t1, TextureProxy* t2 )
{
	return (t2->abs.width * t2->abs.height - t1->abs.width * t1->abs.height) < 0;
}


bool TextureAtlas::create( GLuint* ahandle, GLuint* nhandle, int& width, int& height )
{
	if( internalTextures.size() < 1 ){
		Debug::debug( Debug::GRAPHICS, "Textures is missing.\n" );
		return false;
	}
	// Sort textures in buffer
	sort( internalTextures.begin(), internalTextures.end(), compareTextureProxies );
	// Calculate texture positions and draw to texture
	if( !buildMap( width, height ) ){
		Debug::debug( Debug::GRAPHICS, "Cannot build atlas map.\n" );
		return false;
	}
	if( !build( ahandle, nhandle, width, height ) ){
		Debug::debug( Debug::GRAPHICS, "Cannot draw atlas.\n" );
		return false;
	}

	// Push textures to render and clear array
	Textures::push( internalTextures, *ahandle, nhandle ? *nhandle : 0 );

	clear_vector( &internalTextures );

	return true;
}
