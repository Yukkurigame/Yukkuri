/*
 * Tiles.h
 *
 *  Created on: 27.05.2012
 *
 */

#ifndef TILES_H_
#define TILES_H_

#include "graphics/sprite/Sprite.h"


// Power of two
#define CHUNK_SIZE 3


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
	s2f atlasPos;	// Position in atlas
	s2i pos;		// Position in chunk coordinates
	s2i realPos;	// Position in pixels
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
	s2i screen;
	unsigned int texture;
	void init();
	signed int getFreeSpace( s2f& pos );
	void returnSpace( unsigned int p );
};


#endif /* TILES_H_ */
