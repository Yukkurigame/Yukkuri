#ifndef MAP_H
#define MAP_H

#include "map/Tiles.h"
#include "map/Chunk.h"

enum {
	TILE_TYPE_NONE = 0,
	TILE_TYPE_NORMAL,
	TILE_TYPE_BLOCK
};



struct posOffset
{
	int X;
	int Y;
	posOffset( ){ X = Y = 0; }
	posOffset( int x, int y ) { set(x, y); };

	inline void set( int a, int b ){ X = a; Y = b; }
	inline posOffset flip(){ return posOffset( Y, X ); }

	posOffset operator +( posOffset a ){ return posOffset( a.X + X, a.Y + Y ); }
	void operator +=( posOffset a ){ X += a.X; Y += a.Y; }
	posOffset operator -( posOffset a ){ return posOffset( a.X - X, a.Y - Y ); }
	void operator -=( posOffset a ){ X -= a.X; Y -= a.Y; }

	posOffset operator *( int a ){ return posOffset( X * a, Y * a ); }
	posOffset operator *( posOffset a ){ return posOffset( a.X * X, a.Y * Y ); }
	void operator *=( int a ){ X *= a; Y *= a; }
	void operator *=( posOffset a ){ X *= a.X; Y *= a.Y; }

};

namespace Map
{

	bool init( );
	void clean( );

	unsigned char isActive( );
	void setActive( );
	void clearActive( );

	void toChunkCoordinates( s2i& pos );
	void fromChunkCoordinates( s2i& pos );

	void toMapCoordinates( int* x, int* y);
	void fromMapCoordinates( int* x, int* y);

	void toMapCoordinates( s2i& coord );
	void fromMapCoordinates( s2i& coord );

	MapChunk* createChunk( signed int x, signed int y );
	void deleteChunk( MapChunk* chunk );
	void deleteChunk( signed int x, signed int y );
	inline void deleteChunkp( MapChunk* chunk ) { deleteChunk( chunk ); }


	void deleteTile( signed int x, signed int y );
	void deleteTile( MapTile* tile );
	void deleteTilev( signed int x, signed int y );

	void createHTilesLine( signed int startx, signed int starty, int number );
	void deleteHTilesLine( signed int startx, signed int starty, int number );

	void createVTilesLine( signed int startx, signed int starty, int number );
	void deleteVTilesLine( signed int startx, signed int starty, int number );

	void createChunksRectangle( const s2i& pos, const s2i& count );
	void deleteChunksRectangle( const s2i& pos, const s2i& count );

	MapTile* getTile( float x, float y );
	MapTile* getTile( signed int x, signed int y );

	//ChunkListIter getChunkIt( signed int x, signed int y );
	//ChunkListIter getChunkXIt( signed int x );

	void clear( );

	void onDraw( );

};

#endif //MAP_H
