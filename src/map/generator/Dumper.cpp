/*
 * Dumper.cpp
 *
 *  Created on: 03.04.2013
 */

#include "map/generator/MapGenerator.h"
#include "map/Tiles.h"
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
	if( BSON::pack( data, length, tile ) )
		out->write( data, length );

	delete out;
	delete tile;
}


void dump_center( const char* path, Center* c, int** filled )
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
		while( !filled[w][node.y] && pnpoly( nvert, vertx, verty, w, node.y ) )
			++w;
		while( !filled[e][node.y] && pnpoly( nvert, vertx, verty, e, node.y ) )
			--e;
		for( int i = e; i < w; ++i ){
			dump_chunk( path, i, node.y, c );
			filled[i][node.y] = 1;
			int n = node.y + 1, s = node.y - 1;
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

	if( fwrite( region, sizeof(RegionMap), 1, ptr_fp ) != 1 )
		Debug::debug( Debug::MAP, "Cannot write to region file in %s.\n", path );

	fclose( ptr_fp );
}


void MapGenerator::dumpMap( const char* path )
{
	RegionMap region;

	region.latitude = latitude;
	region.longitude = longitude;
	region.seed = initial_seed;

	int isize = SIZE;
	int** filled = (int**)malloc(sizeof(int)* isize);
	for( int i = 0; i < isize; ++i ){
		filled[i] = (int*)malloc(sizeof(int)* isize);
		memset( filled[i], 0, SIZE );
	}

	Center* c;
	FOREACH1( c, centers ){
		dump_center( path, c, filled );
	}

	for( int i = 0; i < isize; ++i )
		free(filled[i]);
	free(filled);

	// Save region metadata
	save_region( path, &region );
	free( file );

}

