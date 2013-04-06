/*
 * RegionFileCache.h
 *
 *  Created on: 04.04.2013
 */

#ifndef REGIONFILECACHE_H_
#define REGIONFILECACHE_H_

// A simple cache and wrapper for efficiently multiple RegionFiles simultaneously.
#include "map/RegionFile.h"


namespace RegionFileCache
{
	void clean( );

	RegionFile* getRegionFile( const char* basePath, int x, int y );
	int getSizeDelta( const char* basePath, int x, int y );
	int getChunkDataInputStream( char** data, const char* basePath, int x, int y );
	RegionFile::ChunkBuffer* getChunkDataOutputStream( const char* basePath, int x, int y );

};

#endif /* REGIONFILECACHE_H_ */
