/*
 * Chunk.cpp
 *
 *  Created on: 13.02.2013
 *
 */

#include "map/Chunk.h"
#include "map/Map.h"
#include "utils/misc.h"

#include "graphics/Render.h"
#include "graphics/sprite/Mesh.h"
#include "graphics/render/Atlas.h"
#include "graphics/render/GLTextures.h"
#include "graphics/render/GLHelpers.h"
#include "graphics/render/Textures.h"
#include "graphics/utils/ElasticBox.h"

#include "config.h"
#include "debug.h"

#include <cmath>


extern MainConfig conf;


namespace ChunkManager {

	long int state;
	s2i screenCount;
	unsigned int chunksCount;
	s2i chunk_size;
	unsigned int chunkTilesCount;
	s2i screen;
	int tile_size_p2;

	//GLuint atlas;
	//s2i atlasCount;
	//unsigned int texture;
}


MapChunk::MapChunk( signed int x, signed int y )
{
	tiles = NULL;
	sprite = NULL;
	pos.x = realPos.x = x;
	pos.y = realPos.y = y;
	ChunkManager::from_coordinates( realPos );
	unsigned int side = 1 << CHUNK_SIZE;
	//int picture = ChunkManager::get_space( atlasPos );
	//if( picture < 0 )
		//return;
	int count = ChunkManager::chunkTilesCount;
	int mesh = MeshManager::get("mesh_terrain");
	sprite = RenderManager::CreateGLSprite( (float)realPos.x, (float)realPos.y, 0,
								ChunkManager::chunk_size.x, ChunkManager::chunk_size.y, mesh );
	tiles = (MapTile*)malloc( (unsigned)sizeof(MapTile) * count );
	unsigned int row = 0;
	unsigned int col = 0;

	GLBrush& brush = sprite->brush;
	brush.init(-1);

	// Two triangles per tile
	int vx_tiles = 3 * 2;
	int vx_count = count * vx_tiles;
	brush.resize_verticles( vx_count );
	brush.indices_count = 0;
	brush.indices_list = (UINT*)malloc( (UINT)sizeof(UINT) * vx_count );

	VertexV2FT2FC4UI* arr = brush.points();
	GLuint chunk_atlas = 0;
	for( int tile = 0; tile < count; ++tile ){
		MapTile& t = tiles[tile];
		int point_index = tile * vx_tiles;
		int tx = realPos.x + col * conf.mapTileSize; // + ( row % 2 ? (conf.mapTileSize >> 1) : 0 );
		int ty = realPos.y + row * conf.mapTileSize; // - row * ( 3 * (conf.mapTileSize >> 2) );
		int ttx = tx;
		int tty = ty;
		Map::toMapCoordinates( &ttx, &tty );
		t.create( ttx, tty );
		s3f vc[6] = {
			s3f(tx, ty, 0), s3f(tx + conf.mapTileSize, ty, 0),
			s3f(tx + conf.mapTileSize, ty + conf.mapTileSize, 0),
			s3f(tx + conf.mapTileSize, ty + conf.mapTileSize, 0),
			s3f(tx, ty + conf.mapTileSize, 0), s3f(tx, ty, 0)
		};

		for( int i = 0; i < vx_tiles; ++i ){
			int ti = i + point_index;
			arr[ti].verticles = vc[i];
			//arr[ti].coordinates = tc[i];
			//arr[ti].color = s4ub( rand() % 256, rand() % 256, rand() % 256, 255 );
			brush.indices_list[brush.indices_count++] = ti + brush.point_index;
		}

		TextureInfo* texture = Textures::get_pointer(t.Type->texture);
		if( !chunk_atlas )
			chunk_atlas = texture->atlas;
		if( chunk_atlas != texture->atlas )
			Debug::debug( Debug::MAP, "Multiple texture atlases in one chunk. Rewrite this. Tile: %d at %f:%f", t.TileID, t.pos.x, t.pos.y );
		UINT texture_indices[6] = {
				qcBottom, qcRight | qcBottom, qcRight,
				qcRight, 0, qcBottom,
		};
		texture->getSubTexture( t.Type->picture, &arr[point_index], 6, texture_indices );

		if( ++col >= side ){
			col = 0;
			row++;
		}
	}

	if(chunk_atlas){
		sprite->textures.clear();
		sprite->addTexture( chunk_atlas );
	}
}


MapChunk::~MapChunk()
{
	if( sprite ){
		ChunkManager::return_space( sprite->picture );
		RenderManager::FreeGLSprite( sprite );
	}
	if( tiles )
		free( tiles );
}


void ChunkManager::init(){
	// Get pow2 tile size
	tile_size_p2 = floor( log(conf.mapTileSize) * M_LOG2E );
	// Size of one chunk
	chunk_size.x = conf.mapTileSize << CHUNK_SIZE;
	chunk_size.y = conf.mapTileSize << CHUNK_SIZE;

	// Calculate count of chunks in the screen
	screen.x = conf.video.windowWidth >> ( tile_size_p2 + CHUNK_SIZE - 2 );
	screen.y = conf.video.windowHeight >> ( tile_size_p2 + CHUNK_SIZE - 2 );
	// Tiles in chunk: two interpenetrative girds of CHUNK_SIZE^2 tiles.
	chunkTilesCount = 1 << ( CHUNK_SIZE + CHUNK_SIZE );
	// calculate size of atlas. CHUNK_SIZE is additional places here
	chunksCount = next_p2(screen.x * screen.y + CHUNK_SIZE);
	state = 0; // No places occupied

	ElasticRectPODBox box = ElasticRectPODBox( TextureAtlas::getAtlasMax() );
	if( !box.calculate( chunk_size.x, chunk_size.y, chunksCount ) ){
		Debug::debug( Debug::MAP, "Chunks count too big" );
		return;
	}
	chunksCount = box.rows * box.cols;

	//atlas = 0;
	/*TextureProxy tp;
	{
		tp.id = "map_atlas";
		// Too static
		atlasCount.x = tp.cols = box.cols;
		atlasCount.y = tp.rows = box.rows;
		tp.abs.width = tp.cols * chunk_size.x;
		tp.abs.height = tp.rows * chunk_size.y;
		// Texture occupies all atlas
		tp.atlas.width = static_cast<float>(tp.abs.width) / static_cast<float>(box.Width);
		tp.atlas.height = static_cast<float>(tp.abs.height) / static_cast<float>(box.Height);
		tp.atlas.x = tp.atlas.y = 0.0;
	}
	GLTextures::generate( &atlas, box.Width, box.Height );
	texture = Textures::push( &tp, atlas, 0 );
	*/
}


void ChunkManager::to_coordinates( s2i& pos )
{
	//FIXME: Where 1 lost?
	pos.x >>= tile_size_p2 + CHUNK_SIZE + 1;
	pos.y >>= tile_size_p2 + CHUNK_SIZE + 1;
}


void ChunkManager::from_coordinates( s2i& pos )
{
	//FIXME: Where 1 lost?
	pos.x <<= tile_size_p2 + CHUNK_SIZE + 1;
	pos.y <<= tile_size_p2 + CHUNK_SIZE + 1;
}


int ChunkManager::size_p2( )
{
	return tile_size_p2;
}


const s2i& ChunkManager::get_count( )
{
	return screen;
}


// You must call this for request a new free space, not only checking for
signed int ChunkManager::get_space( s2f& pos ){
	unsigned int c = 1, p = 0;
	// Check zeroes
	while( state & c ){
		c <<= 1;
		p++;
		if( p > chunksCount ) // No free space
			return -1;
	}
	//TextureInfo* texinfo = Textures::get_pointer( texture );
	//texinfo->getTexturePosition( pos, p );
	state |= c; // Set free space as occupied
	return p;
}


void ChunkManager::return_space( unsigned int p )
{
	int c = 1;
	for( unsigned int s = 0; s < p; s++ )
		c <<= 1;
	state &= ~c; // Set occupied space as free
}




