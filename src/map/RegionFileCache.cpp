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
#include <stdlib.h>
#include <cerrno>


namespace RegionFileCache
{
	// Not sure if map over strings is faster than char*-tree.
	std::map< std::string, RegionFile* > cache;
}


void RegionFileCache::clean( )
{
	FOREACHIT1( cache ){
		delete it->second;
	}
	cache.clear();
}

RegionFile* RegionFileCache::getRegionFile( const char* basePath, int x, int y )
{
	char* regionDir = Path::join( basePath, "region" );

	char filename[30];
	snprintf( filename, 20, "r%x.%x.yrd", (x >> 5), (y >> 5) );

	char* file = Path::join( regionDir, filename );
	std::string file_str = (std::string)file;

	std::map< std::string, RegionFile* >::iterator fref = cache.find(file_str);
	if( fref != cache.end() && fref->second != NULL ){
		free(regionDir);
		free(file);
		return fref->second;
	}

	struct stat st;
	if( stat( regionDir, &st ) < 0 && errno == ENOENT )
		Path::mkdir_recursive( regionDir, 0744 ); // U+RWX G+R O+R

	RegionFile* reg = new RegionFile( file );
	cache[file_str] = reg;

	free(regionDir);
	free(file);

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


