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
	int TileID;
	int TypeID;
	signed int posX;
	signed int posY;
	bool Blocked;
	Sprite* s;
	MapTile( signed int x, signed int y );
	~MapTile();
};

class Map
{
public:
	Map();
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

	MapTile* GetTile( signed int x, signed int y );

	void Clean( );

	void Draw( );

private:
	bool Updated;
	float posX;
	float posY;

	std::map< signed int, std::map< signed int, MapTile* > > Tiles;
	std::vector< Sprite* > TileSprites;

};

#endif //MAP_H
