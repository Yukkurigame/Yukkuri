/*
 * Chunk.h
 *
 *  Created on: 13.02.2013
 *
 */

#ifndef CHUNK_H_
#define CHUNK_H_

#include "map/Tiles.h"
#include "graphics/sprite/Sprite.h"


// Power of two
#define CHUNK_SIZE 3


struct MapChunk
{
	s2f atlasPos;	// Position in atlas
	s2i pos;		// Position in chunk coordinates
	s2i realPos;	// Position in pixels
	MapTile* tiles;
	Sprite* sprite;
	MapChunk( signed int x, signed int y );
	~MapChunk();

};


namespace ChunkManager {

	static int tile_size_p2;

	void init();
	const s2i& get_count( );
	signed int get_space( s2f& pos );
	void return_space( unsigned int p );

};


#endif /* CHUNK_H_ */
