/*
 * Packer.h
 *
 *  Created on: 10.04.2013
 */

#ifndef MAP_PACKER_H_
#define MAP_PACKER_H_

#include "map/Tiles.h"
#include "map/Chunk.h"


namespace MapPacker
{
	int unpack_region( const char* path, RegionMap* region );
	int unpack_chunk( const char* path, int x, int y, ChunkTile* chunk );
}


#endif /* PACKER_H_ */
