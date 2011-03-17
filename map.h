#ifndef MAP_H
#define MAP_H

#include <map>
#include <vector>
#include <string>

#include "GraphicsTypes.h"

enum {
	TILE_TYPE_NONE = 0,
	TILE_TYPE_NORMAL,
	TILE_TYPE_BLOCK
};

struct MapTile
{
	unsigned int TileID;
	unsigned int TypeID;
	signed int RealX;
	signed int RealY;
	signed int posX;
	signed int posY;
	int BackType;
	float Passability;
	imageRect Image;
	imageRect BackImage;
	MapTile( signed int x, signed int y );
	~MapTile();
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

class Map
{
public:
	Map();
	bool LoadTiles( );
	bool Init( );

	void toMapCoordinates( int* x, int* y);
	void fromMapCoordinates( int* x, int* y);

	MapTile* CreateTile( signed int x, signed int y );
	void DeleteTile( signed int x, signed int y );
	void DeleteTile( MapTile* tile );

	void CreateHTilesLine( signed int startx, signed int starty, int number );
	void DeleteHTilesLine( signed int startx, signed int starty, int number );

	void CreateVTilesLine( signed int startx, signed int starty, int number );
	void DeleteVTilesLine( signed int startx, signed int starty, int number );

	void CreateTilesRectangle( signed int startx, signed int starty, int numberx, int numbery );
	void DeleteTilesRectangle( signed int startx, signed int starty, int numberx, int numbery );

	MapTile* GetTile( float x, float y );
	MapTile* GetTile( signed int x, signed int y );

	void Clean( );

	void Draw( );

private:
	bool Updated;
	float posX;
	float posY;

	std::map< signed int, std::map< signed int, MapTile* > > Tiles;
	std::vector< imageRect* > TileSprites;

};

#endif //MAP_H
