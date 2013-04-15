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
#include <cmath>

char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,
    float p2_x, float p2_y, float p3_x, float p3_y)
{
	s2f s1( p1_x - p0_x, p1_y - p0_y );
	s2f s2( p3_x - p2_x, p3_y - p2_y );

    float s, t;
    s = (-s1.y * (p0_x - p2_x) + s1.x * (p0_y - p2_y)) / (-s2.x * s1.y + s1.x * s2.y);
    t = ( s2.x * (p0_y - p2_y) - s2.y * (p0_x - p2_x)) / (-s2.x * s1.y + s1.x * s2.y);

	if( s >= 0 && s <= 1 && t >= 0 && t <= 1 )
		return 1; // Collision detected

    return 0; // No collision
}

int pnpoly( int nvert, float *vertx, float *verty, float testx, float testy )
{
	int i, j, c = 0;
	for( i = 0, j = nvert - 1; i < nvert; j = i++ ){
		//if( get_line_intersection( vertx[i], verty[i], vertx[j], verty[j]  ) )
		if( ( ( verty[i] > testy ) != ( verty[j] > testy ) )
				&& ( testx < ( vertx[j] - vertx[i] ) * ( testy - verty[i] )
								/ ( verty[j] - verty[i] ) + vertx[i] ) )
			c = !c;
	}
	return c;
}

static float clavgx, clavgy;

struct clockwise
{
	bool operator()( const Corner* A, const Corner* B )
	{
		return ( atan2( A->point.y - clavgy, A->point.x - clavgx )
				< atan2( B->point.y - clavgy, B->point.x - clavgx ) );
	}
};


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

	clavgx = c->point.x;
	clavgy = c->point.y;
	c->corners.sort(clockwise());

	int index = 0;
	ITER_LIST( Corner*, c->corners ) {
		vertx[index] = it->data->point.x;
		verty[index] = it->data->point.y;
		++index;
	}

	// Flood fill
	list< s2f > nodes;
	s2f first;
	first.x = c->point.x;
	first.y = c->point.y;
	nodes.push( first );

	while( nodes.count > 0 ){
		float w, e;
		s2f node = nodes.pop();
		int y = node.y;
		int x = node.x;
		w = e = node.x;
		if( filled[x][y] || !pnpoly( nvert, vertx, verty, x, node.y ) )
			continue;
		while( w < size && pnpoly( nvert, vertx, verty, w, node.y ) )
			++w;
		while( e > 0 && pnpoly( nvert, vertx, verty, e, node.y ) )
			--e;
		if( w > size )
			w = size;
		if( e < 0 )
			e = 0;
		for( int i = e; i < w; ++i ){
			dump_chunk( path, i, y, c );
			filled[i][y] = 1;
			float n = node.y >= size ? size - 1 : node.y + 1;
			float s = node.y > 0 ? node.y - 1 : 0;
			nodes.push( s2f(i, n) );
			nodes.push( s2f(i, s) );
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
	MapProto* proto = get_proto();

	region.latitude = proto->latitude;
	region.longitude = proto->longitude;
	region.seed = proto->initial_seed;
	region.seed_string = proto->initial_seed_string;
	region.texture_id = proto->texture_id;

	int isize = GENERATOR_SIZE;

	int** filled = new int*[isize];
	filled[0] = new int[isize * isize];
	memset( filled[0], 0, sizeof(int) * isize * isize );
	for( int i = 1; i < isize; ++i ){
		filled[i] = filled[0] + i * isize;
	}

	Center* c = NULL;
	char* worldpath = Path::join( path, "world" );
	FOREACH1( c, proto->centers ){
		dump_center( worldpath, c, filled, isize );
	}

	delete[] filled[0];
	delete[] filled;

	// Save region metadata
	save_region( worldpath, &region );

	free( worldpath );
}

