/*
 * RegionFileCache.cpp
 *
 *  Created on: 04.04.2013
 */

#include "map/RegionFileCache.h"
#include "hacks.h"
#include "utils/path.h"
#include "safestring.h"
#include <sys/stat.h>
#include <map>


namespace RegionFileCache
{
	// Not sure if map over strings is faster than char*-tree.
	std::map< std::string, RegionFile* > cache;
}


void RegionFileCache::clear( )
{
	FOREACHIT1( cache ){
		delete it->second;
	}
    cache.clear();
}

RegionFile* RegionFileCache::getRegionFile( const char* basePath, int x, int y )
{
	char* regionDir = Path::join( basePath, "region" );

	char filename[20];
	snprintf( filename, 20, "r%a.%a.yrd", (double)(x >> 5), (double)(y >> 5) );

	char* file = Path::join( regionDir, filename );


	std::map< std::string, RegionFile* >::iterator fref = cache.find((std::string)file);
	if( fref != cache.end() && fref->second != NULL )
		return fref->second;

	mkdir( regionDir, 0644 ); // U+RWX G+R O+R

	RegionFile* reg = new RegionFile( file );
	cache[file] = reg;

	return reg;
}


int RegionFileCache::getSizeDelta( const char* basePath, int x, int y )
{
	RegionFile* r = getRegionFile( basePath, x, y );
	return r->getSizeDelta();
}

int RegionFileCache::getChunkDataInputStream( char** data, const char* basePath, int x, int y )
{
	RegionFile* r = getRegionFile( basePath, x, y );
    return r->getChunkDataInputStream( data, x & 31, y & 31);
}

RegionFile::ChunkBuffer* RegionFileCache::getChunkDataOutputStream( const char* basePath, int x, int y )
{
	RegionFile* r = getRegionFile( basePath, x, y );
	return r->getChunkDataOutputStream( x & 31, y & 31 );
}


