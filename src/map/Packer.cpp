/*
 * Packer.cpp
 *
 *  Created on: 10.04.2013
 */

#include "map/Packer.h"
#include "map/RegionFileCache.h"
#include "utils/path.h"
#include "utils/bson.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>


int MapPacker::unpack_region( const char* path, RegionMap* region )
{

	char* worldpath = Path::join( path, "world" );
	const char* filename = "region.yrd";
	char* filepath = Path::join( worldpath, filename );
	FILE* ptr_fp = fopen( filepath, "rb" );
	free(filepath);

	if( !ptr_fp ){
		Debug::debug( Debug::MAP, "Cannot load region file from %s.\n", path );
		return 0;
	}

	fseek( ptr_fp, 0L, SEEK_END );
	int length = ftell( ptr_fp );
	fseek( ptr_fp, 0L, SEEK_SET );

	// TODO: mmap
	char* data = (char*)malloc( sizeof(char*) * length );
	fread( data, length, 1, ptr_fp );
	int status = 0;

	if( ( status = BSON::unpack( data, region ) ) <= 0 )
			Debug::debug( Debug::MAP, "Cannot read from region file in %s.\n", path );

	fclose( ptr_fp );
	free( worldpath );

	return status;
}


int MapPacker::unpack_chunk( const char* path, int x, int y, ChunkTile* chunk )
{
	char* data = NULL;
	int status = 0;
	char* worldpath = Path::join( path, "world" );
	int len = RegionFileCache::getChunkDataInputStream( &data, worldpath, x, y );

	if( len > 0 ){
		if( ( status = BSON::unpack( data, chunk ) ) <= 0 )
			Debug::debug( Debug::MAP, "Cannot read from chunk file %d:%d in %s.\n", worldpath, x, y );
	}

	free( worldpath );
	return status;
}
