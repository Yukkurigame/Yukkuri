
#include "map/Map.h"
#include "graphics/Camera.h"

#include "config.h"
#include "hacks.h"
#include "debug.h"

#include <math.h>
#include <iterator>


extern MainConfig conf;
extern MapChunkManager ChunkManager;



namespace {

	bool Updated;
	float posX;
	float posY;

	std::vector< MapTile* > Tilesvec;

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
		Map::toMapCoordinates(&ROffset.X, &ROffset.Y);
		ROffset += ROffset; // Right top corner
		LOffset = ROffset + ( Right * -1 * ( XCount >> 1 ) ); // Left top corner

		//FIXME: move from here
		ChunkManager.init();
	}
} Defines;



bool Map::init( )
{
	Updated = false;
	posX = 0;
	posY = 0;
	Defines.Init();
	return true;
}

void Map::clean()
{
	FOREACHIT( Tilesvec ){
		deleteTile( *it );
	}
	Tilesvec.clear( );
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


void Map::toMapCoordinates( s2i& coord )
{
	signed int x, y;
	x = ( 2 * coord.y + coord.x ) / 2;
	y = ( 2 * coord.y - coord.x ) / 2;
	coord.x = x >> Defines.lTileSize;
	coord.y = y >> Defines.lTileSize;
}

void Map::fromMapCoordinates( s2i& coord )
{
	coord.x <<= Defines.lTileSize;
	coord.y <<= Defines.lTileSize;
	coord.x = coord.x - coord.y;
	coord.y = coord.x / 2 + coord.y / 2;
}


MapChunk* Map::createChunk( signed int x, signed int y )
{
	MapChunk* chunk;
	std::vector< MapChunk* >::iterator tit;
	//tit = getTilev(x, y);
	//if( tit != Tilesvec.end() ){
	//	if( (*tit)->pos.x == x && (*tit)->pos.y == y )
	//		return (*tit);
	//}
	//x, y
	chunk = new MapChunk( x, y );
	//Tilesvec.insert(tit, tile);
	Updated = true;
	return chunk;
}

void Map::deleteTile( signed int x, signed int y )
{
	MapTile* tile;
	std::vector< MapTile* >::iterator tit;
	tit = getTilev(x, y);
	if( (*tit)->pos.x == x && (*tit)->pos.y == y )
		Tilesvec.erase( tit );
	tile = getTile( x, y );
	if( !tile )
		return;
	deleteTile( tile );
}

void Map::deleteTile( MapTile* tile )
{
	if( !tile )
		return;
	Updated = true;
	delete tile;
}

MapTile* Map::getTile( float x, float y )
{
	signed int cx = static_cast<int>(floor(x));
	signed int cy = static_cast<int>(floor(y));
	toMapCoordinates( &cx, &cy );
	return getTile( cx, cy );
}

MapTile* Map::getTile( signed int x, signed int y )
{
	std::vector< MapTile* >::iterator m = getTilev(x, y);
	if( m != Tilesvec.end() )
		return *m;
	return NULL;
}

std::vector< MapTile* >::iterator Map::getTilev( signed int x, signed int y )
{
	int first;
	int last;
	int mid;
	int size = Tilesvec.size() - 1;
	std::vector< MapTile* >::iterator begin;

	if( size <= 0 || Tilesvec[size]->pos.x < x )
		return Tilesvec.end();

	begin = Tilesvec.begin();

	if( Tilesvec[0]->pos.x <= x ){
		//First find x
		first = 0;
		last = size;
		while( first < last ){
			mid = first + ( last - first ) / 2;
			if( x <= Tilesvec[mid]->pos.x )
				last = mid;
			else
				first = mid + 1;
		}

		if( Tilesvec[last]->pos.x == x ){
			//Next find y
			first = last;
			while( last < size && Tilesvec[last]->pos.x == x ){
				last++; //All elements with such x
			}
			while( first < last ){
				mid = first + ( last - first ) / 2;
				if( y <= Tilesvec[mid]->pos.y )
					last = mid;
				else
					first = mid + 1;
			}
		}

		std::advance( begin, last );
	}

	return begin;
}

void Map::createHTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		createChunk( startx + i, starty - i );
	}
}

void Map::deleteHTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		deleteTile( startx + i, starty - i );
	}
}

void Map::createVTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		createChunk( startx - i, starty - i );
	}
}

void Map::deleteVTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		deleteTile( startx - i, starty - i );
	}
}

void Map::createTilesRectangle( signed int startx, signed int starty, int numberx, int numbery )
{
	for( int i = 0; i < numberx; ++i ){
		createHTilesLine( startx - i, starty - i, numbery );
		createHTilesLine( startx - i + 1, starty - i, numbery );
	}
}

void Map::deleteTilesRectangle( signed int startx, signed int starty, int numberx, int numbery )
{
	for( int i = 0; i < numberx; ++i ){
		deleteVTilesLine( startx - i, starty - i, numbery );
	}
}

void Map::clear( )
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
		if( t->Real.x > right || t->Real.x < left ||
			t->Real.y > top || t->Real.y < bottom ){
			deleteTile( t );
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
		int halfx = ChunkManager.screenCount.x >> 1;
		for( int x = cx - halfx; x < halfx; x += ChunkManager.chunkSize.x ){
			createChunk( x, 0 );
		}
		//createTilesRectangle( cx + Defines.LOffset.X + Defines.Top.X * 3 - Defines.Right.X * 2,
		//			cy + Defines.LOffset.Y + Defines.Top.Y * 3 - Defines.Right.Y * 2,
		//			Defines.XCount, Defines.YCount);
		posX = cx;
		posY = cy;
	}
	if( Updated ){
		//TODO: cleaning in thread
		clear();
		/*char d[3];
		snprintf(d, 3, "%d\n", Tilesvec.size());
		debug(0, d);*/
		Updated = false;
	}
}

