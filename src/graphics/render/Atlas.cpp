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
#include "graphics/render/GLTextures.h"
#include "graphics/render/Textures.h"
#include "graphics/Render.h"

#include "scripts/LuaConfig.h"

#include "safestring.h"
#include "debug.h"


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
	ITER_LIST( TextureProxy*, internalTextures ){
		delete it->data;
	}
	internalTextures.clear();
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
	lc->pop( 1 );
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
	ITER_LIST( TextureProxy*, internalTextures ){
		TextureProxy* tp = it->data;
		if ( ! box.InsertItem( &(tp->abs.x), &(tp->abs.y), tp->abs.width, tp->abs.height ) )
			return false;
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


inline Sprite* sprite_from_proxy( const TextureProxy* t )
{
	Sprite* s = new Sprite( );
	float x = static_cast<float>(t->offset.x) / static_cast<float>(t->texture->w);
	float y = static_cast<float>(t->offset.y) / static_cast<float>(t->texture->h);
	float dx = static_cast<float>(t->abs.width) / static_cast<float>(t->texture->w);
	float dy = static_cast<float>(t->abs.height) / static_cast<float>(t->texture->h);

	// Order: qcRT=0, qcRB, qcLT, qcLB

	s2f coords[4] = {
			s2f(x + dx, y + dy),
			s2f(x + dx, y),
			s2f(x, y + dy),
			s2f(x, y),
	};
	s3f vertx[4] = {
			s3f(t->abs.x + t->abs.width, t->abs.y + t->abs.height, 0),
			s3f(t->abs.x + t->abs.width, t->abs.y, 0),
			s3f(t->abs.x, t->abs.y + t->abs.height, 0),
			s3f(t->abs.x, t->abs.y, 0)
	};

	VertexV2FT2FC4UI* pts = s->brush.points();
	for( int i = 0; i < s->brush.points_count; ++i ){
		pts[i].coordinates = coords[i];
		pts[i].verticles = vertx[i];
	}
	s->textures.push( t->texture->tex );
	return s;
}


bool TextureAtlas::build( GLuint* ahandle, GLuint* nhandle, int width, int height )
{
	list< Sprite* > sprites;
	ITER_LIST( TextureProxy*, internalTextures ){
		sprites.push_back( sprite_from_proxy(it->data) );
	}

	sprites.sort( compareSprites );

	bool status = GLTextures::draw( ahandle, width, height, &sprites );

	ITER_LIST( Sprite*, sprites ){
		delete it->data;
	}

	return status;
}



inline bool compareTextureProxies( TextureProxy* t1, TextureProxy* t2 )
{
	return (t2->abs.width * t2->abs.height - t1->abs.width * t1->abs.height) < 0;
}


bool TextureAtlas::create( GLuint* ahandle, GLuint* nhandle, int& width, int& height )
{
	if( internalTextures.head == NULL ){
		Debug::debug( Debug::GRAPHICS, "Textures is missing.\n" );
		return false;
	}
	// Sort textures in buffer
	internalTextures.sort( compareTextureProxies );
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
	clean();

	return true;
}
