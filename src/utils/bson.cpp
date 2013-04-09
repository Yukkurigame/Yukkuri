/*
 * bson.cpp
 *
 *  Created on: 05.04.2013
 */

#include "utils/bson.h"
#include "map/Tiles.h"
#include "map/Chunk.h"


#define BINARY(d) (char*)&d

namespace BSON {

int bson_dump( char*& output, bson* b )
{
	int length = bson_size( b );
	output = (char*)malloc( sizeof(char*) * length );
	memcpy( output, bson_data( b ), length );
	return length;
}



template<>
int construct_bson( bson* b, ChunkTile* data )
{
	bson_append_binary( b, "position", 0, BINARY(data->position), sizeof(data->position) );
	bson_append_int( b, "type", data->type );
	return 1;
}


template<>
int construct_bson( bson* b, RegionMap* data )
{
	bson_append_int( b, "seed", data->seed );
	bson_append_double( b, "latitude", data->latitude );
	bson_append_double( b, "longitude", data->longitude );
	return 1;
}

}
