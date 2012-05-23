#include "map.h"
#include "config.h"
#include "Camera.h"
#include "Render.h"
#include "LuaConfig.h"
#include "safestring.h"

#include "hacks.h"

#include <math.h>
#include <iterator>

#include "debug.h"
using namespace Debug;

extern MainConfig conf;

static unsigned int TilesCount = 0;



//FIXME: Я не знаю, почему не вношу это в Map
static unsigned int TileTypesCount = 0;
static std::map< std::string, TileInfo > tiles;
static bool TilesLoaded = false;


Map map;


namespace Region
{

	static std::map< signed int, std::map< signed int, TileInfo* > >  RegionDump;

	void Load( std::string name ){
		debug( MAP, "Loading region " + name + ".\n" );
		if( !TilesLoaded ){
			debug( MAP, "Cannot load region " + name + " without tiles.\n");
			return;
		}
		LuaConfig* cfg = new LuaConfig;
		std::vector< std::map< std::string, std::string > > Tiles;
		cfg->getValue( "tiles", name, "region", Tiles );
		for( std::vector< std::map< std::string, std::string > >::iterator it = Tiles.begin(), end = Tiles.end();
			it != end; ++it ){
			if( it->count( "tile" ) > 0 ){
				int x, y;
				std::string type;
				x = y = 0;
				type = (*it)["tile"];
				if( it->count( "x" ) > 0 )
					x = atoi( (*it)["x"].c_str() );
				if( it->count( "y" ) > 0 )
					y = atoi( (*it)["y"].c_str() );
				RegionDump[x][y] = &tiles[type];
			}
		}
		delete cfg;
	}

	TileInfo* GetTile( signed int x, signed int y ){
		if( RegionDump.count( x ) > 0) {
			if( RegionDump[x].count( y ) > 0 ){
				return RegionDump[x][y];
			}
		}
		return &tiles[conf.mapDefaultTile];
	}
}


static struct MapDefines{
	int XCount;
	int YCount;
	int lTileSize;
	posOffset ROffset;
	posOffset LOffset;
	posOffset Right;
	posOffset Top;
	void Init( ){
		//FIXME: This magic needs to be described
		lTileSize = static_cast<int>( floor( log(static_cast<float>(conf.mapTileSize)) / log(2.0f) ) ) - 1;
		Right.set(1, -1);
		Top.set(1, 1);
		XCount = conf.windowWidth >> lTileSize;
		YCount = conf.windowHeight >> lTileSize;
		ROffset.set( conf.windowWidth / 2, conf.windowHeight / 2 );
		map.toMapCoordinates(&ROffset.X, &ROffset.Y);
		ROffset += ROffset; // Right top corner
		LOffset = ROffset + ( Right * -1 * ( XCount >> 1 ) ); // Left top corner
	}
} Defines;

MapTile::MapTile( signed int x, signed int y ) {
	TileID = TilesCount;
	TilesCount++;

	RealX = posX = x;
	RealY = posY = y;

	Type = Region::GetTile( x, y );

	map.fromMapCoordinates( &RealX, &RealY );

	sprite = NULL;
	sprite = RenderManager::Instance()->CreateGLSprite( RealX - ( conf.mapTileSize >> 1 ),
							RealY - ( conf.mapTileSize >> 2 ), 0,
							conf.mapTileSize, conf.mapTileSize,
							Type->texture, Type->picture );

	//FIXME: я не уверен, что это правильно, но выглядит нормально. Может внезапно вылезти боком.
	//Image.x = RealX - ( conf.mapTileSize >> 1 );
	//Image.y = RealY - ( conf.mapTileSize >> 2 );

}

MapTile::~MapTile( )
{
	RenderManager::Instance()->FreeGLSprite( sprite );
}

Map::Map( )
{
	Updated = false;
	posX = 0;
	posY = 0;
}

Map::~Map( )
{
	FOREACHIT( Tilesvec ){
		DeleteTile( *it );
	}
	Tilesvec.clear( );
}

bool Map::LoadTiles( )
{
	if(TilesLoaded){
		debug( MAP, "Tiles already loaded." );
		return false;
	}
	LuaConfig* cfg = new LuaConfig;
	//FIXME: И тут, внезапно, в функцию врываются костыли.
	std::vector <  std::map < std::string, std::string > > Subconfigs;
	if( !cfg->getSubconfigs( "tile", Subconfigs ) || ! Subconfigs.size() ){
		debug( CONFIG, "Tiles configs opening error or no tiles found.\n");
		return false;
	}else{
		TileTypesCount = Subconfigs.size();
		debug( MAP, "Tiles found: " + citoa(TileTypesCount) + ".\n" );
	}
	for( unsigned int i = 0; i < TileTypesCount; ++i ){
		if( Subconfigs[i].count("id") < 1 ){
			debug( MAP, "Tile have no id.\n" );
			continue;
		}
		std::string id = Subconfigs[i]["id"];
		tiles[id].id = Subconfigs[i]["id"];
		if( Subconfigs[i].count("image") ){
			std::string image = Subconfigs[i]["image"];
			tiles[id].texture = RenderManager::Instance()->GetTextureNumberById( image );
		}
		tiles[id].picture = Subconfigs[i].count("picture") ? atoi(Subconfigs[i]["picture"].c_str()) : 0;
		tiles[id].passability = Subconfigs[i].count("passability") ? atoi(Subconfigs[i]["passability"].c_str()) : 0;
	}

	TilesLoaded = true;

	delete cfg;

	return true;
}


bool Map::Init( )
{
	Defines.Init();
	return true;
}


bool Map::Load(std::string region)
{
	posX = Camera::GetX();
	posY = Camera::GetY();
	Region::Load( region );
	//CreateTilesRectangle( 3, 16, 21, 14 );
	return true;
}


void Map::toMapCoordinates( int* mx, int* my )
{
	signed int x, y;
	x = ( 2 * (*my) + (*mx) ) / 2;
	y = ( 2 * (*my) - (*mx) ) / 2;
	*mx = x >> Defines.lTileSize;
	*my = y >> Defines.lTileSize;
}

void Map::fromMapCoordinates( int* x, int* y )
{
	signed int mx, my;
	*x <<= Defines.lTileSize;
	*y <<= Defines.lTileSize;
	mx = (*x) - (*y);
	my = (*x) / 2 + (*y) / 2;
	*x = mx;
	*y = my;
}

MapTile* Map::CreateTile( signed int x, signed int y )
{
	MapTile* tile;
	std::vector< MapTile* >::iterator tit;
	tit = GetTilev(x, y);
	if( tit != Tilesvec.end() ){
		if( (*tit)->posX == x && (*tit)->posY == y )
			return (*tit);
	}
	tile = new MapTile( x, y );
	Tilesvec.insert(tit, tile);
	Updated = true;
	return tile;
}

void Map::DeleteTile( signed int x, signed int y )
{
	MapTile* tile;
	std::vector< MapTile* >::iterator tit;
	tit = GetTilev(x, y);
	if( (*tit)->posX == x && (*tit)->posY == y )
		Tilesvec.erase( tit );
	tile = GetTile( x, y );
	if( !tile )
		return;
	DeleteTile( tile );
}

void Map::DeleteTile( MapTile* tile )
{
	if( !tile )
		return;
	Updated = true;
	delete tile;
}

MapTile* Map::GetTile( float x, float y )
{
	signed int cx = static_cast<int>(floor(x));
	signed int cy = static_cast<int>(floor(y));
	toMapCoordinates( &cx, &cy );
	return GetTile( cx, cy );
}

MapTile* Map::GetTile( signed int x, signed int y )
{
	std::vector< MapTile* >::iterator m = GetTilev(x, y);
	if( m != Tilesvec.end() )
		return *m;
	return NULL;
}

std::vector< MapTile* >::iterator Map::GetTilev( signed int x, signed int y )
{
	int first;
	int last;
	int mid;
	int size = Tilesvec.size() - 1;
	std::vector< MapTile* >::iterator begin;

	if( size <= 0 || Tilesvec[size]->posX < x )
		return Tilesvec.end();

	begin = Tilesvec.begin();

	if( Tilesvec[0]->posX <= x ){
		//First find x
		first = 0;
		last = size;
		while( first < last ){
			mid = first + ( last - first ) / 2;
			if( x <= Tilesvec[mid]->posX )
				last = mid;
			else
				first = mid + 1;
		}

		if( Tilesvec[last]->posX == x ){
			//Next find y
			first = last;
			while( last < size && Tilesvec[last]->posX == x ){
				last++; //All elements with such x
			}
			while( first < last ){
				mid = first + ( last - first ) / 2;
				if( y <= Tilesvec[mid]->posY )
					last = mid;
				else
					first = mid + 1;
			}
		}

		std::advance( begin, last );
	}

	return begin;
}

void Map::CreateHTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		CreateTile( startx + i, starty - i );
	}
}

void Map::DeleteHTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		DeleteTile( startx + i, starty - i );
	}
}

void Map::CreateVTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		CreateTile( startx - i, starty - i );
	}
}

void Map::DeleteVTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		DeleteTile( startx - i, starty - i );
	}
}

void Map::CreateTilesRectangle( signed int startx, signed int starty, int numberx, int numbery )
{
	for( int i = 0; i < numberx; ++i ){
		CreateHTilesLine( startx - i, starty - i, numbery );
		CreateHTilesLine( startx - i + 1, starty - i, numbery );
	}
}

void Map::DeleteTilesRectangle( signed int startx, signed int starty, int numberx, int numbery )
{
	for( int i = 0; i < numberx; ++i ){
		DeleteVTilesLine( startx - i, starty - i, numbery );
	}
}

void Map::Clean( )
{
	int cx, cy, right, left, top, bottom;
	MapTile* t = NULL;
	cx = Camera::GetX();
	cy = Camera::GetY();
	right = cx + conf.windowWidth + conf.mapTileSize * 3;
	left = cx - ( conf.windowWidth >> 2 );
	top = cy + conf.windowHeight + conf.mapTileSize * 3;
	bottom = cy - ( conf.windowHeight >> 2 );
	for( unsigned int i = 0; i < Tilesvec.size(); ){
		t = Tilesvec[i];
		if( t->RealX > right || t->RealX < left ||
			t->RealY > top || t->RealY < bottom ){
			DeleteTile( t );
			Tilesvec.erase( Tilesvec.begin() + i );
		}else{
			++i;
		}
	}
}


void Map::onDraw( )
{
	int cx, cy;
	cx = static_cast<int>(Camera::GetX());
	cy = static_cast<int>(Camera::GetY());
	toMapCoordinates( &cx, &cy );
	if( posX != cx || posY != cy ){
		//FIXME: избыточность.
		CreateTilesRectangle( cx + Defines.LOffset.X + Defines.Top.X * 3 - Defines.Right.X * 2,
					cy + Defines.LOffset.Y + Defines.Top.Y * 3 - Defines.Right.Y * 2,
					Defines.XCount, Defines.YCount);
		posX = cx;
		posY = cy;
	}
	if( Updated ){
		//TODO: cleaning in thread
		Clean();
		/*char d[3];
		snprintf(d, 3, "%d\n", Tilesvec.size());
		debug(0, d);*/
		Updated = false;
	}
}

