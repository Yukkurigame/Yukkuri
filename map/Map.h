#ifndef MAP_H
#define MAP_H


#include "map/Tiles.h"
#include <vector>



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

	void toMapCoordinates( int* x, int* y);
	void fromMapCoordinates( int* x, int* y);

	void toMapCoordinates( s2i& coord );
	void fromMapCoordinates( s2i& coord );

	MapChunk* createChunk( signed int x, signed int y );
	void deleteTile( signed int x, signed int y );
	void deleteTile( MapTile* tile );
	void deleteTilev( signed int x, signed int y );

	void createHTilesLine( signed int startx, signed int starty, int number );
	void deleteHTilesLine( signed int startx, signed int starty, int number );

	void createVTilesLine( signed int startx, signed int starty, int number );
	void deleteVTilesLine( signed int startx, signed int starty, int number );

	void createTilesRectangle( signed int startx, signed int starty, int numberx, int numbery );
	void deleteTilesRectangle( signed int startx, signed int starty, int numberx, int numbery );

	MapTile* getTile( float x, float y );
	MapTile* getTile( signed int x, signed int y );

	std::vector< MapTile* >::iterator getTilev( signed int x, signed int y );

	void clear( );

	void onDraw( );

};

#endif //MAP_H
