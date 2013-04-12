/*
 * bson.cpp
 *
 *  Created on: 05.04.2013
 */

#include "utils/bson.h"
#include "map/Tiles.h"
#include "map/Chunk.h"
#include "debug.h"


#define BINARY(d) (char*)&d

#define IF_MATCH( string, type, action )				\
	if( strcmp( string, bson_iterator_key( it ) ) == 0 ){ \
		if( bson_iterator_type( it ) != type ){			\
			Debug::debug( Debug::MAIN, "Bson type does not match for key %s.\n", string );	\
			continue;									\
		}												\
		action											\
	}


namespace BSON {

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
	bson_append_string( b, "seed_string", data->seed_string );
	bson_append_double( b, "latitude", data->latitude );
	bson_append_double( b, "longitude", data->longitude );
	return 1;
}


int construct_data_binary( bson_iterator* iter, void* output, size_t output_size )
{
	size_t len = bson_iterator_bin_len( iter );
	if( len != output_size ){
		Debug::debug( Debug::MAIN, "Bson binary data size does not match destination size.\n" );
		return 0;
	}
	memcpy( output, bson_iterator_bin_data( iter ), len );
	return 1;
}


template<>
int construct_data( bson_iterator* iter, int & data )
{
	data = bson_iterator_int( iter );
	return 1;
}


template<>
int construct_data( bson_iterator* iter, float & data )
{
	data = bson_iterator_double( iter );
	return 1;
}


template<>
int construct_data( bson_iterator* iter, char* & data )
{
	if( data != NULL ){
		Debug::debug( Debug::MAIN, "Target char* is not NULL while bson string requested.\n" );
		return 0;
	}
	data = strdup(bson_iterator_string( iter ));
	return 1;
}


template<>
int construct_data( bson_iterator* iter, Biome& data )
{
	data = (Biome)bson_iterator_int( iter );
	return 1;
}


int dump_bson( char*& output, bson* b )
{
	int length = bson_size( b );
	output = (char*)malloc( sizeof(char*) * length );
	memcpy( output, bson_data( b ), length );
	return length;
}


template<>
int dump_bson( RegionMap* data, bson* b )
{
	bson_iterator it[1];
    bson_iterator_init( it, b );
    int count = 0;
    while( bson_iterator_next( it ) ) {
		IF_MATCH( "latitude", BSON_DOUBLE,
			count += construct_data( it, data->latitude ); )
		IF_MATCH( "latitude", BSON_DOUBLE,
			count += construct_data( it, data->longitude ); )
		IF_MATCH( "seed", BSON_INT,
			count += construct_data( it, data->seed ); )
		IF_MATCH( "seed_string", BSON_INT,
			count += construct_data( it, data->seed_string ); )
    }
	return count;
}

template<>
int dump_bson( ChunkTile* data, bson* b )
{
	bson_iterator it[1];
	bson_iterator_init( it, b );
	int count = 0;
	while( bson_iterator_next( it ) ) {
		IF_MATCH( "type", BSON_INT,
			count += construct_data( it, data->type ); )
		IF_MATCH( "position", BSON_BINDATA,
			count += construct_data_binary( it, &data->position, sizeof( data->position ) ); )
	}
	return count;
}

}
