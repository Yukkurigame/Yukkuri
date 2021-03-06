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

struct RegionMap
{
	float latitude;
	float longitude;
	int seed;
	char* seed_string;
	UINT texture_id;
	RegionMap() : seed_string(0) {}
};


enum {
	TILE_TYPE_NONE = 0,
	TILE_TYPE_NORMAL,
	TILE_TYPE_BLOCK
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
