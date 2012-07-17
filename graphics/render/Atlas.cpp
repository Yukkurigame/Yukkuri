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




namespace {

	std::vector < TextureProxy* > internalTextures;

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
	std::string config = "sprite";
	TextureProxy* t = new TextureProxy();

	lc->getValue("id", name, config, t->id);
	lc->getValue("image", name, config, t->image);
	lc->getValue("width", name, config, t->abs.width);
	lc->getValue("height", name, config, t->abs.height);
	lc->getValue("rows", name, config, t->rows);
	lc->getValue("columns", name, config, t->cols);
	lc->getValue("offsetx", name, config, t->offset.x);
	lc->getValue("offsety", name, config, t->offset.y);

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
	FOREACHIT( internalTextures ){
		if ( ! box.InsertItem(
				&((*it)->abs.x), &((*it)->abs.y),
				(*it)->abs.width, (*it)->abs.height ) )
			return false;
	}

	width = box.Width;
	height = box.Height;

	return buildRelativeMap( box.Width, box.Height );
}


bool TextureAtlas::buildRelativeMap( float width, float height ){

	float texelW = 1.0f;
	float texelH = 1.0f;

	texelW = texelW / width;
	texelH = texelH / height;

	// Build relative map
	for( unsigned int i = 0; i < internalTextures.size(); i++ ){
		internalTextures[i]->atlas.x = static_cast<float>(internalTextures[i]->abs.x) * texelW;
		internalTextures[i]->atlas.y = static_cast<float>(internalTextures[i]->abs.y) * texelH;
		internalTextures[i]->atlas.width = static_cast<float>(internalTextures[i]->abs.width) * texelW;
		internalTextures[i]->atlas.height = static_cast<float>(internalTextures[i]->abs.height) * texelH;
	}

	return true;
}


bool TextureAtlas::build( GLuint* ahandle, int width, int height )
{
	return TextureArray::drawToNewGLTexture( ahandle, width, height, internalTextures );
}



inline bool compareTextureProxies( TextureProxy* t1, TextureProxy* t2 )
{
	return t2->abs.width * t2->abs.height - t1->abs.width * t1->abs.height;
}


bool TextureAtlas::create( GLuint* ahandle, int& width, int& height )
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
	if( !build( ahandle, width, height ) ){
		Debug::debug( Debug::GRAPHICS, "Cannot draw atlas.\n" );
		return false;
	}

	// Push textures to render and clear array
	RenderManager::PushTextures( internalTextures, *ahandle );
	clear_vector( &internalTextures );

	return true;
}
