#include "map.h"
#include "config.h"
#include "Camera.h"
#include "Graphics.h"
#include "Luaconfig.h"
#include <math.h>

extern MainConfig conf;
static unsigned int TilesCount = 0;
//FIXME: Я не знаю, почему не вношу это в Map
static unsigned int TileTypesCount = 0;
static imageRect* TilesArray = NULL;
static bool TilesLoaded = false;

Map map;

static int compareRects( const void * a, const void * b )
{
    const imageRect* aI = static_cast<const imageRect*>(a);
    const imageRect* bI = static_cast<const imageRect*>(b);
    if( aI && bI )
    	return ( aI->id - bI->id );
    else
    	return 0;
}

namespace Region
{

	static std::map< signed int, std::map< signed int, unsigned int > >  RegionDump;
	static std::map< signed int, std::map< signed int, unsigned int > >  RegionBackDump;

	void Load( std::string name ){
		debug( 5, "Loading region " + name + ".\n" );
		bool Backing;
		char BackName[3];
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
				//FIXME: А как?
				sprintf( BackName, "%d", type );
				LuaConfig::Instance()->getValue( "backing", BackName, "tiles", Backing );
				if( Backing ){
					if( it->count( "backtype" ) > 0 )
						RegionBackDump[x][y] = (*it)["backtype"];
					else
						RegionBackDump[x][y] = conf.mapDefaultTile;
				}
			}
		}
	}

	unsigned int GetTile( signed int x, signed int y ){
		if( RegionDump.count( x ) > 0) {
			if( RegionDump[x].count( y ) > 0 ){
				return RegionDump[x][y];
			}
		}
		return conf.mapDefaultTile;
	}

	unsigned int GetTileBack( signed int x, signed int y ){
		if( RegionBackDump.count( x ) > 0) {
			if( RegionBackDump[x].count( y ) > 0 ){
				return RegionBackDump[x][y];
			}
		}
		return 0;
	}
}

/* Return tile with passed id.
 * If id not found and tiles array exists its return first tile.
 * Otherwise returns tile with -1 as id.
 */
static inline imageRect getTileById( int tileid )
{
	if( TilesLoaded && TilesArray ){
		int first;
		int last;
		int mid;

		if( TilesArray[0].id <= tileid && TilesArray[TileTypesCount-1].id >= tileid ){
			first = 0;
			last = TileTypesCount;
			while( first < last ){
				mid = first + ( last - first ) / 2;
				if( tileid <= TilesArray[mid].id )
					last = mid;
				else
					first = mid + 1;
			}
			if( TilesArray[last].id == tileid )
				return TilesArray[last];
		}
		char d[30];
		sprintf( d, "Tile with id %d not found.\n", tileid );
		debug( 3, d );
		return TilesArray[0];
	}
	debug( 3, "Tiles not loaded.\n" );
	imageRect fake;
	return fake;
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
		lTileSize = static_cast<int>( log(conf.mapTileSize) / log(2) );
		Right.set(1, -1);
		Top.set(1, 1);
		XCount = conf.windowWidth >> lTileSize;
		YCount = conf.windowHeight >> lTileSize;
		ROffset.set( conf.windowWidth / 2, conf.windowHeight / 2 );
		map.toMapCoordinates(&ROffset.X, &ROffset.Y);
		ROffset += ROffset;
		LOffset = ROffset + ( Right * -1 * ( XCount >> 1 ) );
		//Now (rox, roy) - right top corner
		//Left-bottom (-rox, -roy)
		//Left-top (loy, lox)
		//Right-bottom (-loy, -lox)
	}
} Defines;

//TODO:Одноуровневая карта, высокие объекты занимают 2 клетки вместо одной.

MapTile::MapTile( signed int x, signed int y ) {
	TileID = TilesCount;
	TilesCount++;

	RealX = posX = x;
	RealY = posY = y;

	TypeID = Region::GetTile( x, y );
	if( TypeID > TileTypesCount )
		TypeID = 0;

	Image = getTileById( TypeID );

	BackType = Region::GetTileBack(x, y);

	map.fromMapCoordinates( &RealX, &RealY );

	//FIXME: я не уверен, что это правильно, но выглядит нормально. Может внезапно вылезти боком.
	Image.x = RealX - ( conf.mapTileSize >> 1 );
	Image.y = RealY - ( conf.mapTileSize >> 2 );

	if( BackType ){
		BackImage = getTileById( BackType );
		BackImage.x = RealX - ( conf.mapTileSize >> 1 );
		BackImage.y = RealY - ( conf.mapTileSize >> 2 );
		BackImage.z -= 0.01;
	}


	if( TypeID ){
		//FIXME: ororoshenkiroro
		char name[ sizeof(TypeID) ];
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
	//TileTypesCount++; // First tile are blank;
	TilesArray = (imageRect*)malloc( sizeof(imageRect) * ( TileTypesCount ) );
	TilesArray[0].id = 0;
	memset(TilesArray[0].imageName, 0, 65);
	TilesArray[0].x = 0;
	TilesArray[0].y = 0;
	TilesArray[0].z = 0;
	TilesArray[0].coordinates = NULL;
	TilesArray[0].width = TilesArray[0].height = conf.mapTileSize;
	for( unsigned int i = 0; i < TileTypesCount; ++i ){
		char name[100];
		memset(name, 0, 65);
		if( Subconfigs[i-1].count("image") )
			strcpy(name, Subconfigs[i]["image"].c_str());
		TilesArray[i].id =  Subconfigs[i].count("name") ? atoi(Subconfigs[i]["name"].c_str()) : 0;
		strcpy( TilesArray[i].imageName, name );
		TilesArray[i].x = Subconfigs[i].count("offsetx") ? atof(Subconfigs[i]["offsetx"].c_str()) : 0;
		TilesArray[i].y = Subconfigs[i].count("offsety") ? atof(Subconfigs[i]["offsety"].c_str()) : 0;
		TilesArray[i].z = 0;
		TilesArray[i].coordinates = NULL;
		TilesArray[i].width = TilesArray[i].height = conf.mapTileSize;
	}
	qsort(TilesArray, TileTypesCount, sizeof(imageRect), compareRects);
	Graphics::Instance()->CreateGLTextureAtlas( conf.mapTileSize, &TilesArray[0], TileTypesCount );

	TilesLoaded = true;

	return true;
}

bool Map::Init( )
{
	posX = YCamera::CameraControl.GetX();
	posY = YCamera::CameraControl.GetY();
	Region::Load("test");
	Defines.Init();
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
	tile = GetTile(x, y);
	if( tile )
		return tile;
	tile = new MapTile( x, y );
	Tiles[x][y] = tile;
	if( tile->Image.texture )
		TileSprites.push_back( &tile->Image );
	if( tile->BackType )
		TileSprites.push_back( &tile->BackImage );
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
	for( vector< imageRect* >::iterator it = TileSprites.begin(), end = TileSprites.end(); it != end; ++it ){
		//FIXME: null pointer error?
		if( (*it) == &tile->Image ){
			TileSprites.erase( it );
			break;
		}
	}
	if( tile->BackType ){
		for( vector< imageRect* >::iterator it = TileSprites.begin(), end = TileSprites.end(); it != end; ++it ){
			//FIXME: null pointer error?
			if( (*it) == &tile->BackImage ){
				TileSprites.erase( it );
				break;
			}
		}
	}
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
	MapTile* t;
	cx = YCamera::CameraControl.GetX();
	cy = YCamera::CameraControl.GetY();
	right = cx + conf.windowWidth + conf.mapTileSize * 3;
	left = cx - ( conf.windowWidth >> 2 );
	top = cy + conf.windowHeight + conf.mapTileSize * 3;
	bottom = cy - ( conf.windowHeight >> 2 );
	for( std::map< signed int, std::map< signed int, MapTile* > >::iterator it = Tiles.begin(),
			end = Tiles.end(); it != end; ++it ){
		for( std::map< signed int, MapTile* >::iterator vit = it->second.begin(), vend = it->second.end();
							vit != vend; ++vit ){
			t = vit->second;
			if( t->RealX > right || t->RealX < left ||
				t->RealY > top || t->RealY < bottom ){
				DeleteTile( vit->second );
				vit->second = NULL;
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
		//FIXME: избыточность.
		if( posX < cx || posY > cy ){ // move right or bottom
			CreateTilesRectangle( cx + Defines.ROffset.X - Defines.Right.X *
										( Defines.XCount >> 2 ) + Defines.Top.X * 3,
								  cy + Defines.ROffset.Y - Defines.Right.Y *
										( Defines.XCount >> 2 ) + Defines.Top.X * 3,
								  Defines.XCount, Defines.YCount >> 1 );
			CreateTilesRectangle( cx + Defines.LOffset.X/2 - Defines.Right.X * 3,
								  cy + Defines.LOffset.Y/2 - Defines.Right.Y * 3,
								  Defines.XCount >> 1, Defines.YCount);
			posX = cx;
			posY = cy;
		}else if( posX > cx || posY < cy ){ //move left or top
			CreateTilesRectangle( cx + Defines.LOffset.X + Defines.Top.X * 3 - Defines.Right.X * 2,
								  cy + Defines.LOffset.Y + Defines.Top.Y * 3 - Defines.Right.Y * 2,
								  Defines.XCount, Defines.YCount >> 1);
			CreateTilesRectangle( cx + Defines.LOffset.X + Defines.Top.X * 3 - Defines.Right.X * 2,
								  cy + Defines.LOffset.Y + Defines.Top.Y * 3 - Defines.Right.Y * 2,
								  Defines.XCount >> 1, Defines.YCount);
			posX = cx;
			posY = cy;
		}
	}
	if( Updated ){
		//TODO: cleaning in thread
		Clean();
		/*char d[3];
		sprintf(d, "%d\n", TileSprites.size());
		debug(0, d);*/
		Graphics::Instance()->AddImageRectArray( &TileSprites );
		Updated = false;
	}
}

