/*
 * Tiles.cpp
 *
 *  Created on: 27.05.2012
 *
 */

#include "map/Tiles.h"
#include "map/Region.h"
#include "map/Map.h"
#include "graphics/render/TextureArray.h"
#include "graphics/render/GLHelpers.h"
#include "graphics/Render.h"

#include "config.h"

#include <math.h>


extern MainConfig conf;

MapChunkManager ChunkManager;

// Power of two
#define CHUNK_SIZE 3


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
	// tile y is half of x; Each odd row located between two another
	chunkSize.y = conf.mapTileSize << (CHUNK_SIZE - 1);
	// Calculate count of chunks in the screen
	screenCount.x = conf.windowWidth >> ( lTileSize + CHUNK_SIZE - 1 );
	screenCount.y = conf.windowHeight >> ( lTileSize + CHUNK_SIZE - 1 );
	// Tiles in atlas
	chunkTilesCount = 1 << ( CHUNK_SIZE + CHUNK_SIZE );
	// calculate size of atlas. CHUNK_SIZE is additional places here
	chunksCount = next_p2(screenCount.x * screenCount.y + CHUNK_SIZE);
	state = 0; // No places occupied

	TextureProxy tp;
	{
		tp.id = "map_atlas";
		// Too static
		atlasCount.x = tp.cols = next_p2(CHUNK_SIZE);
		atlasCount.y = tp.rows = chunksCount / tp.cols;
		tp.abs.width = tp.cols * chunkSize.x;
		tp.abs.height = tp.rows * chunkSize.y;
		// Texture occupies all atlas
		tp.atlas.width = tp.atlas.height = 1.0;
		tp.atlas.x = tp.atlas.y = 0.0;
	}
	GLHelpers::CreateTexture( &atlas, tp.abs.width, tp.abs.height );
	texture = RenderManager::Instance()->PushTexture( &tp, atlas );
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
	pos.y = p / atlasCount.x;
	pos.x = p - pos.y * atlasCount.x;
	state |= c; // Set free space as occupied
	return p;
}

void MapChunkManager::returnSpace( unsigned int p )
{
	int c = 1;
	for( unsigned int s = 0; s < p; s++ )
		c <<= 1;
	state |= ~c; // Set occupied space as free
}


MapChunk::MapChunk( signed int x, signed int y )
{
	tiles = NULL;
	sprite = NULL;
	unsigned int side = 1 << CHUNK_SIZE;
	int picture = ChunkManager.getFreeSpace( atlasPos );
	if( picture < 0 )
		return;
	sprite = RenderManager::Instance()->CreateGLSprite( x, y, 0, ChunkManager.chunkSize.x, ChunkManager.chunkSize.y, ChunkManager.texture, picture );
	tiles = (MapTile*)malloc( sizeof(MapTile) * ChunkManager.chunkTilesCount );
	unsigned int row = 0;
	unsigned int col = 0;
	Sprite sprites[ChunkManager.chunkTilesCount];
	for( unsigned int tile = 0; tile  < ChunkManager.chunkTilesCount; ++tile ){
		MapTile& t = tiles[tile];
		t.create( x + col, y + row );
		Sprite& s = sprites[tile];
		s.texid = t.Type->texture;
		s.tex = RenderManager::Instance()->GetTextureByNumber( s.texid );
		s.atlas = s.tex->atlas;
		s.setPosition(
				atlasPos.x + col * conf.mapTileSize + ( row % 2 ? (conf.mapTileSize >> 1) : 0 ),
				atlasPos.y + row * conf.mapTileSize - row * ( 3 * (conf.mapTileSize >> 2) ) );
		s.setPicture( t.Type->picture );
		s.resize( conf.mapTileSize, conf.mapTileSize );
		if( ++col >= side ){
			col = 0;
			row++;
		}
	}
	Texture tex;
	tex.w = ChunkManager.chunkSize.x;
	tex.h = ChunkManager.chunkSize.y;
	tex.tex = 0;
	TextureArray::drawToNewGLTexture( &tex.tex, ChunkManager.chunkSize.x, ChunkManager.chunkSize.y, sprites, ChunkManager.chunkTilesCount );
	GLHelpers::UpdateTexture( ChunkManager.atlas, &tex, atlasPos.x * ChunkManager.chunkSize.x, atlasPos.y * ChunkManager.chunkSize.y );
}



MapChunk::~MapChunk()
{
	if( sprite ){
		ChunkManager.returnSpace( sprite->picture );
		RenderManager::Instance()->FreeGLSprite( sprite );
	}
	if( tiles )
		free( tiles );
}



