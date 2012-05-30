/*
 * Tiles.h
 *
 *  Created on: 27.05.2012
 *
 */

#ifndef TILES_H_
#define TILES_H_

#include "GraphicsTypes.h"


struct TileInfo
{
	unsigned int picture;
	float passability;
	std::string id;
	int texture;
	TileInfo() : picture(), passability(), texture() {};
};


struct MapTile
{
	unsigned int TileID;
	s2i Real;
	s2i pos;
	TileInfo* Type;
	void create( signed int x, signed int y );
	MapTile( ) : TileID() {}
};


struct MapChunk
{
	s2f atlasPos;
	MapTile* tiles;
	Sprite* sprite;
	MapChunk( signed int x, signed int y );
	~MapChunk();

};

struct MapChunkManager {
	long int state;
	s2i screenCount;
	s2i atlasCount;
	unsigned int chunksCount;
	s2i chunkSize;
	unsigned int chunkTilesCount;
	GLuint atlas;
	unsigned int texture;
	void init();
	signed int getFreeSpace( s2f& pos );
	void returnSpace( unsigned int p );
};


#endif /* TILES_H_ */
