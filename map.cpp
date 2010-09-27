#include "map.h"
#include "config.h"
#include "Camera.h"
#include "Graphics.h"
#include "Luaconfig.h"
#include <math.h>

extern MainConfig conf;
static int TilesCount = 0;

Map map;

namespace Region
{

	static std::map< signed int, std::map< signed int, int > >  RegionDump;

	void Load( std::string name ){
		debug( 5, "Loading region " + name + ".\n" );
		std::vector< std::map< string, int > > Tiles;
		LuaConfig::Instance()->getValue( "tiles", name, "mapregion", Tiles );
		//Lua cannot into number keys
		for( std::vector< std::map< string, int > >::iterator it = Tiles.begin(), end = Tiles.end();
			it != end; ++it ){
			if( it->count( "type" ) > 0 ){
				int x, y;
				x = y = 0;
				if( it->count( "x" ) > 0 )
					x = (*it)["x"];
				if( it->count( "y" ) > 0 )
					y = (*it)["y"];
				RegionDump[x][y] = (*it)["type"];
			}
		}
	}

	int GetTile( signed int x, signed int y ){
		if( RegionDump.count( x ) > 0) {
			if( RegionDump[x].count( y ) > 0 ){
				return RegionDump[x][y];
			}
		}
		return 1;
	}
}

MapTile::MapTile( signed int x, signed int y ) {
	float offsetx, offsety;
	char name[3];
	string image;

	TileID = TilesCount;
	TilesCount++;

	posX = x;
	posY = y;

	offsetx = offsety = 0;

	TypeID = Region::GetTile( x, y );
	sprintf( name, "%d", TypeID );

	LuaConfig::Instance()->getValue( "image", name, "tiles", image );
	LuaConfig::Instance()->getValue( "offsetx", name, "tiles", offsetx );
	LuaConfig::Instance()->getValue( "offsety", name, "tiles", offsety );
	LuaConfig::Instance()->getValue( "blocked", name, "tiles", Blocked );
	s = Graphics::Instance()->CreateGLSprite(
			round(x * conf.mapTileSize + ( ( y % 2 == 0 ) ? conf.mapTileSize/2 : 0 )),
			round(y * conf.mapTileSize * 0.3), 0, offsetx, offsety,
			conf.mapTileSize, conf.mapTileSize * 0.6,
			Graphics::Instance()->LoadGLTexture( image ), 0, 0, 0 );
	s->fixed = false;
}

MapTile::~MapTile( )
{
	Graphics::Instance()->FreeGLSprite( s );
	s = NULL;
}

Map::Map( )
{
	Updated = false;
	posX = 0;
	posY = 0;
}

bool Map::Init( )
{
	posX = YCamera::CameraControl.GetX();
	posY = YCamera::CameraControl.GetY();
	Region::Load("test");
	CreateTilesRectangle( -20, -30, 40, 60 );
	return true;
}

MapTile* Map::CreateTile( signed int x, signed int y )
{
	MapTile* tile;
	tile = GetTile(x, y);
	if( tile ){
		return tile;
	}
	tile = new MapTile( x, y );
	Tiles[x][y] = tile;
	TileSprites.push_back( tile->s );
	Updated = true;
	return tile;
}

void Map::DeleteTile( signed int x, signed int y )
{
	MapTile* tile;
	tile = GetTile( x, y );
	if( !tile )
		return;
	Tiles[x].erase( Tiles[x].find(y) );
	DeleteTile( tile );
}

void Map::DeleteTile( MapTile* tile )
{
	if( !tile )
		return;
	Updated = true;
	for( vector< Sprite* >::iterator it = TileSprites.begin(), end = TileSprites.end(); it != end; ++it ){
		if( (*it) == tile->s ){
			TileSprites.erase( it );
			break;
		}
	}
	delete tile;
}

MapTile* Map::GetTile( signed int x, signed int y )
{
	if( Tiles.count( x ) > 0) {
		if( Tiles[x].count( y ) > 0 ){
			return Tiles[x][y];
		}
	}
	return NULL;
}

void Map::CreateHTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		CreateTile( startx + i, starty );
	}
}

void Map::DeleteHTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		DeleteTile( startx + i, starty );
	}
}

void Map::CreateVTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		CreateTile( startx, starty + i );
	}
}

void Map::DeleteVTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		DeleteTile( startx, starty + i );
	}
}

void Map::CreateTilesRectangle( signed int startx, signed int starty, int numberx, int numbery )
{
	for( int i = 0; i < numberx; ++i ){
		CreateVTilesLine( startx + i, starty, numbery );
	}
}

void Map::DeleteTilesRectangle( signed int startx, signed int starty, int numberx, int numbery )
{
	for( int i = 0; i < numberx; ++i ){
		DeleteVTilesLine( startx + i, starty, numbery );
	}
}

void Map::Clean( )
{
	int cx, cy;
	cx = YCamera::CameraControl.GetX();
	cy = YCamera::CameraControl.GetY();
	for( std::map< signed int, std::map< signed int, MapTile* > >::iterator it = Tiles.begin(),
			end = Tiles.end(); it != end; ++it ){
		if( it->first > ( round( (cx + conf.windowWidth ) / conf.mapTileSize ) + 5 ) ||
				it->first < ( round( (cx - conf.windowWidth / 2 ) / conf.mapTileSize ) ) ){
			for( std::map< signed int, MapTile* >::iterator vit = it->second.begin(), vend = it->second.end();
								vit != vend; ++vit ){
				DeleteTile( vit->second );
				vit->second = NULL;
				it->second.erase( vit );
			}
		}else{
			for( std::map< signed int, MapTile* >::iterator vit = it->second.begin(), vend = it->second.end();
					vit != vend; ++vit ){
				if( vit->first > ( round( ( cy + conf.windowHeight ) / ( conf.mapTileSize * 0.3 ) ) + 10 ) ||
					vit->first < ( round( ( cy - conf.windowHeight*0.4 ) / (conf.mapTileSize * 0.3) ) - 10 ) ){
					DeleteTile( vit->second );
					vit->second = NULL;
				}
				if( !vit->second )
					it->second.erase( vit );
			}
		}
		if( it->second.empty() )
			Tiles.erase( it );
	}
}

void Map::Draw( )
{
	int cx, cy;
	cx = YCamera::CameraControl.GetX();
	cy = YCamera::CameraControl.GetY();
	if( posX != cx || posY != cy ){
		//TODO: cleaning in thread
		if( posX < cx - conf.mapTileSize * 5 ){
			CreateTilesRectangle( round( (cx + conf.windowWidth  ) / conf.mapTileSize ),
					round( (cy + conf.windowHeight * 0.06 ) / ( conf.mapTileSize * 0.3 ) ) - 13,
					5, conf.windowHeight / ( conf.mapTileSize * 0.3 ) + 26 );
			posX = cx;
		}else if( posX > cx + conf.mapTileSize * 5 ){
			CreateTilesRectangle( round( (cx - conf.windowWidth / 2 ) / conf.mapTileSize ),
					round( (cy + conf.windowHeight * 0.06 ) / ( conf.mapTileSize * 0.3 ) ) - 13,
					5, conf.windowHeight / ( conf.mapTileSize * 0.3 ) + 26 );
			posX = cx;
		}
		if( posY < cy - conf.mapTileSize * 2.5 ){
			CreateTilesRectangle( round( cx / conf.mapTileSize ) - 6,
					round( (cy + conf.windowHeight ) / ( conf.mapTileSize * 0.3 ) ),
					conf.windowWidth / conf.mapTileSize + 12, 10);
			posY = cy;
		}else if( posY > cy + conf.mapTileSize * 2.5 ){
			CreateTilesRectangle( round( cx / conf.mapTileSize ) - 6,
					round( (cy - conf.windowHeight * 0.4 ) / ( conf.mapTileSize * 0.3 ) ),
					conf.windowWidth / conf.mapTileSize + 12, 10);
			posY = cy;
		}
	}
	if( Updated ){
		Clean();
		/*char d[3];
		sprintf(d, "%d\n", TileSprites.size());
		debug(0, d);*/
		Graphics::Instance( )->CreateGLSpriteList( &TileSprites );
		Updated = false;
	}
}
