/*
 * Dumper.cpp
 *
 *  Created on: 03.04.2013
 */

#include "map/generator/MapGenerator.h"
#include "map/Tiles.h"
#include "map/Chunk.h"
#include "map/RegionFileCache.h"
#include "hacks.h"
#include "utils/path.h"
#include "safestring.h"
#include "debug.h"
#include <stdlib.h>
#include "utils/bson.h"


int pnpoly( int nvert, float *vertx, float *verty, float testx, float testy )
{
	int i, j, c = 0;
	for( i = 0, j = nvert - 1; i < nvert; j = i++ ){
		if( ( ( verty[i] > testy ) != ( verty[j] > testy ) )
				&& ( testx < ( vertx[j] - vertx[i] ) * ( testy - verty[i] )
								/ ( verty[j] - verty[i] ) + vertx[i] ) )
			c = !c;
	}
	return c;
}


void dump_chunk( const char* path, int x, int y, Center* c )
{
	ChunkTile* tile = new ChunkTile;
	tile->position.x = x;
	tile->position.y = y;
	tile->type = c->biome;

	RegionFile* region = RegionFileCache::getRegionFile( path, x, y );
	RegionFile::ChunkBuffer* out = region->getChunkDataOutputStream( x & 31, y & 31 );

	char* data = NULL;
	int length = 0;
	if( BSON::pack( data, length, tile ) > 0 )
		out->write( data, length );

	delete out;
	delete tile;
}


void dump_center( const char* path, Center* c, int**& filled, int size )
{
	// Prepare slow check
	int nvert = c->corners.count;
	float vertx[nvert];
	float verty[nvert];

	int index = 0;
	ITER_LIST( Corner*, c->corners ) {
		vertx[index] = it->data->point.x;
		verty[index] = it->data->point.y;
		++index;
	}

	// Flood fill
	list< s2i > nodes;
	s2i first;
	first.x = c->point.x;
	first.y = c->point.y;
	nodes.push( first );

	while( nodes.count > 0 ){
		s2i node = nodes.pop();
		int w, e;
		if( filled[node.x][node.y] || !pnpoly( nvert, vertx, verty, node.x, node.y ) )
			continue;
		w = e = node.x;
		while( w < size && !filled[w][node.y] && pnpoly( nvert, vertx, verty, w, node.y ) )
			++w;
		while( e > 0 && !filled[e][node.y] && pnpoly( nvert, vertx, verty, e, node.y ) )
			--e;
		for( int i = e; i < w; ++i ){
			dump_chunk( path, i, node.y, c );
			filled[i][node.y] = 1;
			int n = node.y >= size ? size - 1 : node.y + 1;
			int s = node.y > 0 ? node.y - 1 : 0;
			if( !filled[i][n] )
				nodes.push( s2i(i, n) );
			if( !filled[i][s] )
				nodes.push( s2i(i, s) );
		}
	}
}

void save_region( const char* path, RegionMap* region )
{
	const char* filename = "region.yrd";

	char* filepath = Path::join( path, filename );
	FILE* ptr_fp = fopen( filepath, "wb" );
	free(filepath);

	if( !ptr_fp ){
		Debug::debug( Debug::MAP, "Cannot create region file in %s.\n", path );
		return;
	}

	int length = 0;
	char* data = NULL;

	if( BSON::pack( data, length, region ) <= 0 ||
		fwrite( data, length, 1, ptr_fp ) != 1 ){
			Debug::debug( Debug::MAP, "Cannot write to region file in %s.\n", path );
	}

	fclose( ptr_fp );
}


void MapGenerator::dumpMap( const char* path )
{
	RegionMap region;

	region.latitude = latitude;
	region.longitude = longitude;
	region.seed = initial_seed;
	region.seed_string = initial_seed_string;
	int isize = SIZE;

	int** filled = new int*[isize];
	filled[0] = new int[isize * isize];
	memset( filled[0], 0, sizeof(int) * isize * isize );
	for( int i = 1; i < isize; ++i ){
		filled[i] = filled[0] + i * isize;
	}

	Center* c = NULL;
	char* worldpath = Path::join( path, "world" );
	FOREACH1( c, centers ){
		dump_center( worldpath, c, filled, isize );
	}

	delete[] filled[0];
	delete[] filled;

	// Save region metadata
	save_region( worldpath, &region );

	free( worldpath );
}

