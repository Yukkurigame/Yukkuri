/*
 * Tiles.cpp
 *
 *  Created on: 27.05.2012
 *
 */

#include "map/Tiles.h"
#include "map/Region.h"
#include "map/Map.h"
#include "graphics/render/GLTextures.h"
#include "graphics/render/GLHelpers.h"
#include "graphics/render/Textures.h"
#include "graphics/render/ElasticBox.h"
#include "graphics/render/Atlas.h"
#include "graphics/Render.h"

#include "config.h"
#include "debug.h"

#include <math.h>



extern MainConfig conf;

MapChunkManager ChunkManager;


namespace {

	unsigned int TilesCount = 0;

	//FIXME: dublicate
	inline int next_p2( int a )
	{
		int rval=2;
		while( rval < a ) rval <<= 1;
		return rval;
	}

}



void MapTile::create( signed int x, signed int y )
{
	TileID = TilesCount;
	TilesCount++;

	Real.x = pos.x = x;
	Real.y = pos.y = y;

	Type = Region::getTile( x, y );

	Map::fromMapCoordinates( Real );

	//sprite = NULL;
	//sprite = RenderManager::Instance()->CreateGLSprite( RealX - ( conf.mapTileSize >> 1 ),
							//RealY - ( conf.mapTileSize >> 2 ), 0,
							//conf.mapTileSize, conf.mapTileSize,
							//Type->texture, Type->picture );

	//FIXME: я не уверен, что это правильно, но выглядит нормально. Может внезапно вылезти боком.
	//Image.x = RealX - ( conf.mapTileSize >> 1 );
	//Image.y = RealY - ( conf.mapTileSize >> 2 );
}


void MapChunkManager::init(){
	atlas = 0;
	// Get pow2 tile size
	int lTileSize = static_cast<int>( floor( log(static_cast<float>(conf.mapTileSize)) / log(2.0f) ) ) - 1;
	// Size of one chunk
	// 0.5 of tile for second row offset
	chunkSize.x = (conf.mapTileSize << CHUNK_SIZE) + (conf.mapTileSize >> 1);
	// tile y is half of x; Each odd row located between two another.
	chunkSize.y = ( conf.mapTileSize << (CHUNK_SIZE - 1 ) ) + (conf.mapTileSize >> 1);
	// Calculate count of chunks in the screen
	screen.x = conf.video.windowWidth >> ( lTileSize + CHUNK_SIZE );
	screen.y = conf.video.windowHeight >> ( lTileSize + CHUNK_SIZE - 1 );
	// Tiles in chunk: two interpenetrative girds of CHUNK_SIZE^2 tiles.
	chunkTilesCount = 1 << ( CHUNK_SIZE + CHUNK_SIZE + 1);
	// calculate size of atlas. CHUNK_SIZE is additional places here
	chunksCount = next_p2(screen.x * screen.y + CHUNK_SIZE);
	state = 0; // No places occupied

	ElasticRectPODBox box = ElasticRectPODBox( TextureAtlas::getAtlasMax() );
	if( !box.calculate( chunkSize.x, chunkSize.y, chunksCount ) ){
		Debug::debug( Debug::MAP, "Chunks count too big" );
		return;
	}
	chunksCount = box.rows * box.cols;
	TextureProxy tp;
	{
		tp.id = "map_atlas";
		// Too static
		atlasCount.x = tp.cols = box.cols;
		atlasCount.y = tp.rows = box.rows;
		tp.abs.width = tp.cols * chunkSize.x;
		tp.abs.height = tp.rows * chunkSize.y;
		// Texture occupies all atlas
		tp.atlas.width = static_cast<float>(tp.abs.width) / static_cast<float>(box.Width);
		tp.atlas.height = static_cast<float>(tp.abs.height) / static_cast<float>(box.Height);
		tp.atlas.x = tp.atlas.y = 0.0;
	}
	GLTextures::generate( &atlas, box.Width, box.Height );
	texture = Textures::push( &tp, atlas, 0 );
}

// You must call this for request a new free space, not only checking for
signed int MapChunkManager::getFreeSpace( s2f& pos ){
	unsigned int c = 1, p = 0;
	// Check zeroes
	while( state & c ){
		c <<= 1;
		p++;
		if( p > chunksCount ) // No free space
			return -1;
	}
	TextureInfo* texinfo = Textures::get_pointer( texture );
	texinfo->getTexturePosition( pos, p );
	state |= c; // Set free space as occupied
	return p;
}

void MapChunkManager::returnSpace( unsigned int p )
{
	int c = 1;
	for( unsigned int s = 0; s < p; s++ )
		c <<= 1;
	state &= ~c; // Set occupied space as free
}


MapChunk::MapChunk( signed int x, signed int y )
{
	tiles = NULL;
	sprite = NULL;
	pos.x = realPos.x = x;
	pos.y = realPos.y = y;
	Map::fromChunkCoordinates( realPos );
	unsigned int side = 1 << CHUNK_SIZE;
	int picture = ChunkManager.getFreeSpace( atlasPos );
	if( picture < 0 )
		return;
	sprite = RenderManager::CreateGLSprite( (float)realPos.x, (float)realPos.y, 0,
			ChunkManager.chunkSize.x, ChunkManager.chunkSize.y, ChunkManager.texture, picture );
	//sprite->shader = Shaders::getProgram( "lighting" );
	tiles = (MapTile*)malloc( (unsigned)sizeof(MapTile) * ChunkManager.chunkTilesCount );
	unsigned int row = 0;
	unsigned int col = 0;
	Sprite* sprites = new Sprite[ ChunkManager.chunkTilesCount ];
	list< Sprite* > sprite_list;
	for( int tile = ChunkManager.chunkTilesCount - 1; tile >= 0; --tile ){
		MapTile& t = tiles[tile];
		int tx = realPos.x + col * conf.mapTileSize + ( row % 2 ? (conf.mapTileSize >> 1) : 0 );
		int ty = realPos.y + row * conf.mapTileSize - row * ( 3 * (conf.mapTileSize >> 2) );
		Map::toMapCoordinates( &tx, &ty );
		t.create( tx, ty );

		Sprite* s = &sprites[tile];
		sprite_list.push( s );
		s->texid = t.Type->texture;
		TextureInfo* tex = Textures::get_pointer( s->texid );
		s->textures.push_back( tex->atlas );
		s->addNormalMap( tex->normals );
		s->setPosition(
				(float)( col * conf.mapTileSize + ( row % 2 ? (conf.mapTileSize >> 1) : 0 ) ),
				(float)( row * conf.mapTileSize - row * ( 3 * (conf.mapTileSize >> 2) ) ) );
		s->setPicture( t.Type->picture );
		s->resize( (float)conf.mapTileSize, (float)conf.mapTileSize );
		if( ++col >= side ){
			col = 0;
			row++;
		}
	}
	Texture tex;
	tex.w = ChunkManager.chunkSize.x;
	tex.h = ChunkManager.chunkSize.y;
	tex.tex = 0;
	GLTextures::draw( &tex.tex, ChunkManager.chunkSize.x, ChunkManager.chunkSize.y,
						&sprite_list, true );
	GLHelpers::UpdateTexture( ChunkManager.atlas, &tex, (int)atlasPos.x, (int)atlasPos.y );
	delete[] sprites;

}


MapChunk::~MapChunk()
{
	if( sprite ){
		ChunkManager.returnSpace( sprite->picture );
		RenderManager::FreeGLSprite( sprite );
	}
	if( tiles )
		free( tiles );
}



