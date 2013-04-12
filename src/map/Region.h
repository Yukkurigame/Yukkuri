/*
 * Region.h
 *
 *  Created on: 27.05.2012
 *
 */

#ifndef REGION_H_
#define REGION_H_

#include "map/Tiles.h"
#include "map/Chunk.h"


namespace Region
{
	bool init( );
	void clean( );
	void generate( UINT form, const char* seed );
	int load( const char* name );
	void unload( );
	float get_latitude( );
	float get_longitude( );
	const char* get_seed( );
	TileInfo* getTile( signed int x, signed int y );
	ChunkTile* getChunk( signed int x, signed int y );

}


#endif /* REGION_H_ */
