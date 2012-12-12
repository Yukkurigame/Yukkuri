
#include "map/Map.h"
#include "graphics/Camera.h"

#include "config.h"
#include "hacks.h"
#include "debug.h"

#include <math.h>
#include <iterator>
#include <algorithm>


extern MainConfig conf;
extern MapChunkManager ChunkManager;



namespace {

	bool Updated;
	float posX;
	float posY;
	int map_flags;

	std::vector< MapChunk* > chunkVec;

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
	map_flags = 0;
	Defines.Init();
	return true;
}

void Map::clean()
{
	FOREACHIT( chunkVec ){
		deleteChunk( *it );
	}
	chunkVec.clear( );
}


unsigned char Map::isActive()
{
	return map_flags & 1;
}

void Map::setActive()
{
	map_flags |= 1;
}

void Map::clearActive()
{
	map_flags &= ~1;
}


void Map::toChunkCoordinates( int& x, int& y)
{
	x >>= Defines.lTileSize + CHUNK_SIZE + 1;
	y >>= Defines.lTileSize + CHUNK_SIZE;
}

void Map::fromChunkCoordinates( s2i& pos )
{
	pos.x <<= Defines.lTileSize + CHUNK_SIZE + 1;
	pos.y <<= Defines.lTileSize + CHUNK_SIZE;
}


void Map::toMapCoordinates( int* mx, int* my )
{
	signed int x, y;
	x = ( 2 * (*my) + (*mx) ) / 2;
	y = ( 2 * (*my) - (*mx) ) / 2;
	*mx = x >> Defines.lTileSize;
	*my = y >> Defines.lTileSize;
}

void Map::toMapCoordinates( s2i& coord )
{
	coord.x = ( ( 2 * coord.y + coord.x ) / 2 ) >> Defines.lTileSize;
	coord.y = ( ( 2 * coord.y - coord.x ) / 2 ) >> Defines.lTileSize;
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


void Map::fromMapCoordinates( s2i& coord )
{
	signed int mx;
	coord.x <<= Defines.lTileSize;
	coord.y <<= Defines.lTileSize;
	mx = coord.x - coord.y;
	coord.y = coord.x / 2 + coord.y / 2;
	coord.x = mx;
}


MapChunk* Map::createChunk( signed int x, signed int y )
{
	MapChunk* chunk;
	ChunkListIter tit;
	tit = getChunkIt( x, y );
	if( tit != chunkVec.end() ){
		if( (*tit)->pos.x == x && (*tit)->pos.y == y )
			return (*tit);
	}
	//x, y
	chunk = new MapChunk( x, y );
	chunkVec.insert( tit, chunk );
	Updated = true;
	return chunk;
}


void Map::deleteChunk( MapChunk* chunk )
{
	if( !chunk )
		return;
	Updated = true;
	delete chunk;
}


void Map::deleteChunk( signed int x, signed int y )
{
	ChunkListIter tit;
	tit = getChunkIt(x, y);
	if( (*tit)->pos.x == x && (*tit)->pos.y == y ){
		deleteChunk( *tit );
		chunkVec.erase( tit );
	}
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
	//std::vector< MapTile* >::iterator m = getTilev(x, y);
	//if( m != Tilesvec.end() )
	//	return *m;
	return NULL;
}

ChunkListIter Map::getChunkIt( signed int x, signed int y )
{
	int first;
	int last;
	int mid;
	int size = chunkVec.size() - 1;
	ChunkListIter begin;

	if( size <= 0 || chunkVec[size]->pos.x < x )
		return chunkVec.end();

	begin = chunkVec.begin();

	if( chunkVec[0]->pos.x <= x ){
		//First find x
		first = 0;
		last = size;
		while( first < last ){
			mid = first + ( last - first ) / 2;
			if( x <= chunkVec[mid]->pos.x )
				last = mid;
			else
				first = mid + 1;
		}

		if( chunkVec[last]->pos.x == x ){
			//Next find y
			first = last;
			while( last < size && chunkVec[last]->pos.x == x ){
				last++; //All elements with such x
			}
			while( first < last ){
				mid = first + ( last - first ) / 2;
				if( y <= chunkVec[mid]->pos.y )
					last = mid;
				else
					first = mid + 1;
			}
		}

		std::advance( begin, last );
	}

	return begin;
}

ChunkListIter Map::getChunkXIt( signed int x )
{
	int first;
	int last;
	int mid;
	int size = chunkVec.size() - 1;
	ChunkListIter begin;

	if( size <= 0 || chunkVec[size]->pos.x < x )
		return chunkVec.end();

	begin = chunkVec.begin();

	if( chunkVec[0]->pos.x <= x ){
		//First find x
		first = 0;
		last = size;
		while( first < last ){
			mid = first + ( last - first ) / 2;
			if( x <= chunkVec[mid]->pos.x )
				last = mid;
			else
				first = mid + 1;
		}

		std::advance( begin, last );
	}

	return begin;
}

void Map::createHTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		createChunk( startx + i, starty );
	}
}

void Map::deleteHTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		deleteChunk( startx + i, starty );
	}
}

void Map::createVTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		createChunk( startx - i, starty );
	}
}

void Map::deleteVTilesLine( signed int startx, signed int starty, int number )
{
	for( int i = 0; i < number; ++i ){
		deleteChunk( startx - i, starty );
	}
}

void Map::createChunksRectangle( signed int startx, signed int starty, int numberx, int numbery )
{
	for( int i = 0; i < numbery; ++i ){
		createHTilesLine( startx, starty + i , numberx );
	}
}

void Map::deleteChunksRectangle( signed int startx, signed int starty, int numberx, int numbery )
{
	for( int i = 0; i < numberx; ++i ){
		deleteVTilesLine( startx - i, starty - i, numbery );
	}
}

void Map::clear( )
{
	int cx, cy, ytop;
	if( chunkVec.empty() )
		return;
	cx = static_cast<int>(Camera::GetX()) - 64;
	cy = static_cast<int>(Camera::GetY()) - 64;
	toChunkCoordinates( cx, cy );
	ytop = cy + ChunkManager.screen.y - 1;
	ChunkListIter xlborder = getChunkXIt( cx );
	if( xlborder != chunkVec.end() ){
		// Remove chunks before visible X
		std::for_each( chunkVec.begin(), xlborder, deleteChunkp );
		xlborder = chunkVec.erase( chunkVec.begin(), xlborder );
	}
	ChunkListIter xrborder = getChunkXIt( cx + ChunkManager.screen.x );
	if( xrborder != chunkVec.end() ){
		// Remove chunks after visible X
		std::for_each( xrborder, chunkVec.end(), deleteChunkp );
		xrborder = chunkVec.erase( xrborder, chunkVec.end() );
	}
	// Remove unvisible Y chunks
	while( xlborder != chunkVec.end() ){
		MapChunk* c = *xlborder;
		if( c->pos.y < cy || c->pos.y > ytop ){
			deleteChunk( c );
			xlborder = chunkVec.erase( xlborder );
		}else{
			++xlborder;
		}
	}
	Updated = false;
}


void Map::onDraw( )
{
	if( ! isActive() )
		return;
	if( Updated ){
		//TODO: cleaning in thread
		clear();
		/*char d[3];
		snprintf(d, 3, "%d\n", Tilesvec.size());
		debug(0, d);*/
		Updated = false;
	}
	int cx, cy;
	cx = static_cast<int>(Camera::GetX()) - 64;
	cy = static_cast<int>(Camera::GetY()) - 64;
	toChunkCoordinates( cx, cy );
	if( posX != cx || posY != cy ){
		createChunksRectangle( cx, cy, ChunkManager.screen.x, ChunkManager.screen.y );
		posX = (float)cx;
		posY = (float)cy;
	}
}

