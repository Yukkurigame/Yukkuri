/*
 * Tiles.h
 *
 *  Created on: 27.05.2012
 *
 */

#ifndef TILES_H_
#define TILES_H_

#include "basic_types.h"
#include "map/Constants.h"

struct ChunkTile
{
	s3f position;
	Biome type;
};

struct RegionTile
{
	ChunkTile tiles[REGION_CHUNK_SIZE][REGION_CHUNK_SIZE];
};


struct RegionMap
{
	float latitude;
	float longitude;
	int seed;
	RegionTile** tiles;
};




struct TileInfo
{
	unsigned int picture;
	float passability;
	int texture;
	char* id;
	TileInfo() : picture(), passability(), texture(), id(0) {};
};



struct MapTile
{
	unsigned int TileID;
	s2i Real;
	s2i pos;
	TileInfo* Type;
	void create( signed int x, signed int y );
	MapTile( ) : TileID(), Type(0) {};

};



#endif /* TILES_H_ */
