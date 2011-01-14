#include "map.h"
#include "config.h"
#include "Camera.h"
#include "Graphics.h"
#include "Luaconfig.h"
#include <math.h>

extern MainConfig conf;
static int TilesCount = 0;
//FIXME: Я не знаю, почему не вношу это в Map
static int TileTypesCount = 0;
static imageRect* TilesArray = NULL;
static bool TilesLoaded = false;

Map map;

namespace Region
{

	static std::map< signed int, std::map< signed int, int > >  RegionDump;
	static std::map< signed int, std::map< signed int, int > >  RegionBackDump;

	void Load( std::string name ){
		debug( 5, "Loading region " + name + ".\n" );
		bool Backing;
		char BackName[3];
		std::map< int, bool > TileBack;
		std::vector< std::map< string, int > > Tiles;
		LuaConfig::Instance()->getValue( "tiles", name, "mapregion", Tiles );
		//Lua cannot into number keys
		for( std::vector< std::map< string, int > >::iterator it = Tiles.begin(), end = Tiles.end();
			it != end; ++it ){
			if( it->count( "type" ) > 0 ){
				int x, y, type;
				x = y = 0;
				type = (*it)["type"];
				if( it->count( "x" ) > 0 )
					x = (*it)["x"];
				if( it->count( "y" ) > 0 )
					y = (*it)["y"];
				RegionDump[x][y] = type;

				//FIXME: Убрать этот костыль, перенести в imageRect
				if( TileBack.count( type ) > 0 ){
					Backing = TileBack[type];
				}else{
					sprintf( BackName, "%d", type );
					LuaConfig::Instance()->getValue( "backing", BackName, "tiles", Backing );
				}
				if( Backing ){
					if( it->count( "backtype" ) > 0 )
						RegionBackDump[x][y] = (*it)["backtype"];
					else
						RegionBackDump[x][y] = conf.mapDefaultTile;
					TileBack[type] = true;
				}
			}
		}
	}

	int GetTile( signed int x, signed int y ){
		if( RegionDump.count( x ) > 0) {
			if( RegionDump[x].count( y ) > 0 ){
				return RegionDump[x][y];
			}
		}
		return conf.mapDefaultTile;
	}

	int GetTileBack( signed int x, signed int y ){
		if( RegionBackDump.count( x ) > 0) {
			if( RegionBackDump[x].count( y ) > 0 ){
				return RegionBackDump[x][y];
			}
		}
		return 0;
	}
}

static struct MapDefines{
	int OffsetX;
	int OffsetY;
	int xXCount;
	int xYCount;
	int yXCount;
	int yYCount;
	void Init( ){
		OffsetX = conf.windowWidth >> 7;
		OffsetY = ( conf.windowHeight/2 + 32 ) >> 5;
		xXCount = conf.windowWidth >> 6;
		xYCount = ( conf.windowHeight + 32 ) >> 3;
		yXCount = conf.windowWidth >> 5;
		yYCount = conf.windowHeight >> 5;
	}
} Defines;

//TODO:Одноуровневая карта, высокие объекты занимают 2 клетки вместо одной.

MapTile::MapTile( signed int x, signed int y ) {
	int backtype;
	TileID = TilesCount;
	TilesCount++;

	posX = x;
	posY = y;
	Backing = false;

	TypeID = Region::GetTile( x, y );
	if( TypeID > TileTypesCount )
		TypeID = 0;

	Image = TilesArray[TypeID];

	backtype = Region::GetTileBack(x, y);

	char name[ sizeof(TypeID) ];

	map.fromMapCoordinates( &x, &y );


	if( TypeID ){
		//FIXME: ororoshenkiroro
		memset( name, 0, sizeof(name) );
		sprintf( name, "%d", TypeID );
		LuaConfig::Instance()->getValue( "passability", name, "tiles", Passability );
	}
}

MapTile::~MapTile( )
{
	//Graphics::Instance()->FreeGLSprite( Image );
	//Image = NULL;
	//Graphics::Instance()->FreeGLSprite( BackImage );
	//BackImage = NULL;
}

Map::Map( )
{
	Updated = false;
	posX = 0;
	posY = 0;
}

bool Map::LoadTiles( )
{
	char dbg[25];
	if(TilesLoaded){
		debug( 5, "Tiles already loaded." );
		return false;
	}
	//FIXME: И тут, внезапно, в функцию врываются костыли.
	std::vector <  std::map < string, string > > Subconfigs;
	if( !LuaConfig::Instance()->getSubconfigs( "tiles", Subconfigs ) || ! Subconfigs.size() ){
		debug(1, "Tiles configs opening error or no tiles found.\n");
		return false;
	}else{
		TileTypesCount = Subconfigs.size();
		sprintf( dbg, "Tiles found: %d\n", Subconfigs.size() );
		debug( 5, dbg );
	}
	TileTypesCount++; // First tile are blank;
	TilesArray = (imageRect*)malloc( sizeof(imageRect) * ( TileTypesCount ) );
	TilesArray[0].id = 0;
	memset(TilesArray[0].imageName, 0, 65);
	TilesArray[0].x = 0;
	TilesArray[0].y = 0;
	for( int i = 1; i < TileTypesCount; ++i ){
		char name[100];
		memset(name, 0, 65);
		if( Subconfigs[i-1].count("image") )
			strcpy(name, Subconfigs[i-1]["image"].c_str());
		TilesArray[i].id =  Subconfigs[i-1].count("name") ? atoi(Subconfigs[i-1]["name"].c_str()) : 0;
		strcpy( TilesArray[i].imageName, name );
		TilesArray[i].x = Subconfigs[i-1].count("offsetx") ? atof(Subconfigs[i-1]["offsetx"].c_str()) : 0;
		TilesArray[i].y = Subconfigs[i-1].count("offsety") ? atof(Subconfigs[i-1]["offsety"].c_str()) : 0;
	}
	Graphics::Instance()->CreateGLTextureAtlas( conf.mapTileSize, &TilesArray[0], TileTypesCount );

	return true;
}

bool Map::Init( )
{
	posX = YCamera::CameraControl.GetX();
	posY = YCamera::CameraControl.GetY();
	Region::Load("test");
	Defines.Init();
	CreateTilesRectangle( -20, -30, 40, 60 );
	return true;
}

void Map::toMapCoordinates( int* x, int* y )
{
	//FIXME: static tile size
	*x >>= 6;
	*y >>= 4;
}

void Map::fromMapCoordinates( int* x, int* y )
{
	//FIXME: static tile size
	*x = ( (*x) << 6 ) + ( ( (*y) & 1 ) << 5 );
	*y <<= 4;
}

MapTile* Map::CreateTile( signed int x, signed int y )
{
	MapTile* tile;
	tile = GetTile(x, y);
	if( tile )
		return tile;
	tile = new MapTile( x, y );
	Tiles[x][y] = tile;
	/*if( tile->Image )
		TileSprites.push_back( tile->Image );
	if( tile->BackImage )
		TileSprites.push_back( tile->BackImage );
	*/
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
		//FIXME: null pointer error?
		/*if( (*it) == tile->Image ){
			TileSprites.erase( it );
			break;
		}*/
	}
	/*if( tile->BackImage ){
		for( vector< Sprite* >::iterator it = TileSprites.begin(), end = TileSprites.end(); it != end; ++it ){
			//FIXME: null pointer error?
			if( (*it) == tile->BackImage ){
				TileSprites.erase( it );
				break;
			}
		}
	}*/
	delete tile;
}

MapTile* Map::GetTile( float x, float y )
{
	signed int cx = static_cast<int>(x);
	signed int cy = static_cast<int>(y);
	toMapCoordinates( &cx, &cy );
	return GetTile( cx, cy );
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
	toMapCoordinates( &cx, &cy );
	for( std::map< signed int, std::map< signed int, MapTile* > >::iterator it = Tiles.begin(),
			end = Tiles.end(); it != end; ++it ){
		if( it->first > ( cx + Defines.xXCount + 5 ) ||
				it->first < ( cx - Defines.OffsetX ) ){
			for( std::map< signed int, MapTile* >::iterator vit = it->second.begin(), vend = it->second.end();
								vit != vend; ++vit ){
				DeleteTile( vit->second );
				vit->second = NULL;
				it->second.erase( vit );
			}
		}else{
			for( std::map< signed int, MapTile* >::iterator vit = it->second.begin(), vend = it->second.end();
					vit != vend; ++vit ){
				if( vit->first > ( cy + ( ( conf.windowHeight - 32 ) >> 4 ) + 5 ) ||
					vit->first < cy - Defines.OffsetY - 4 ){
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
	toMapCoordinates( &cx, &cy );
	if( posX != cx || posY != cy ){
		//TODO: cleaning in thread
		if( posX < cx - 4 ){
			CreateTilesRectangle( cx + Defines.OffsetX, cy - Defines.OffsetY,
									Defines.xXCount, Defines.xYCount );
			posX = cx;
		}else if( posX > cx + 4 ){
			CreateTilesRectangle( cx - Defines.OffsetX, cy - Defines.OffsetY,
									Defines.xXCount, Defines.xYCount);
			posX = cx;
		}
		if( posY < cy - 2 ){
			CreateTilesRectangle( cx - Defines.OffsetX, cy + Defines.yYCount,
									Defines.yXCount, Defines.yYCount + 4);
			posY = cy;
		}else if( posY > cy + 2 ){
			CreateTilesRectangle( cx - Defines.OffsetX, cy - Defines.OffsetY,
									Defines.yXCount, Defines.yYCount + 4);
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

