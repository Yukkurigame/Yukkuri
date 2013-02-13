
#include "map/Map.h"

#include "graphics/Camera.h"
#include "3rdparty/timer/TimerManager.h"
#include "debug.h"
#include "hacks.h"

#include <vector>
#include <cmath>


#define MAP_UPADTE_PERIOD 100
typedef std::vector< MapChunk* >::iterator ChunkListIter;


namespace Map {

	bool Updated;
	s2i position;
	int map_flags;

	std::vector< MapChunk* > chunkVec;
	ChunkListIter get_chunk_iter( signed int x, signed int y );


	class MapCaller : public ITimerEventPerformer
	{
		int event_id;
	public:
		MapCaller() : event_id(-1) {};
		virtual ~MapCaller(){ };

		inline void start( ) {
			if( event_id < 0 )
				event_id = Timer::AddInternalTimerEvent(
						this, 1, MAP_UPADTE_PERIOD, 0, true, true );
			Timer::ResumeTimerEvent( event_id );
		}
		inline void stop( ) {
			Timer::SuspendTimerEvent( event_id );
		}

		inline void OnTimer( InternalTimerEvent& ev ){
			Map::onDraw();
		}

		void OnTimerEventDestroy( const InternalTimerEvent& ev ){	}
	} map_caller;

}

/*
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
		XCount = conf.video.windowWidth >> lTileSize;
		YCount = conf.video.windowHeight >> lTileSize;
		ROffset.set( conf.video.windowWidth / 2, conf.video.windowHeight / 2 );
		Map::toMapCoordinates(&ROffset.X, &ROffset.Y);
		ROffset += ROffset; // Right top corner
		LOffset = ROffset + ( Right * -1 * ( XCount >> 1 ) ); // Left top corner

		//FIXME: move from here
		ChunkManager::init();
	}
} Defines;
*/


bool Map::init( )
{
	Updated = false;
	position.x = 0;
	position.y = 0;
	map_flags = 0;
	ChunkManager::init();
	//Defines.Init();
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
	map_caller.start();
}

void Map::clearActive()
{
	map_flags &= ~1;
	map_caller.stop();
}


void Map::toChunkCoordinates( s2i& pos )
{
	pos.x >>= ChunkManager::tile_size_p2 + CHUNK_SIZE + 1;
	pos.y >>= ChunkManager::tile_size_p2 + CHUNK_SIZE;
}


void Map::fromChunkCoordinates( s2i& pos )
{
	pos.x <<= ChunkManager::tile_size_p2 + CHUNK_SIZE + 1;
	pos.y <<= ChunkManager::tile_size_p2 + CHUNK_SIZE;
}


void Map::toMapCoordinates( int* mx, int* my )
{
	signed int x, y;
	x = ( 2 * (*my) + (*mx) ) / 2;
	y = ( 2 * (*my) - (*mx) ) / 2;
	*mx = x >> ChunkManager::tile_size_p2;
	*my = y >> ChunkManager::tile_size_p2;
}

void Map::toMapCoordinates( s2i& coord )
{
	coord.x = ( ( 2 * coord.y + coord.x ) / 2 ) >> ChunkManager::tile_size_p2;
	coord.y = ( ( 2 * coord.y - coord.x ) / 2 ) >> ChunkManager::tile_size_p2;
}

void Map::fromMapCoordinates( int* x, int* y )
{
	signed int mx, my;
	*x <<= ChunkManager::tile_size_p2;
	*y <<= ChunkManager::tile_size_p2;
	mx = (*x) - (*y);
	my = (*x) / 2 + (*y) / 2;
	*x = mx;
	*y = my;
}


void Map::fromMapCoordinates( s2i& coord )
{
	signed int mx;
	coord.x <<= ChunkManager::tile_size_p2;
	coord.y <<= ChunkManager::tile_size_p2;
	mx = coord.x - coord.y;
	coord.y = coord.x / 2 + coord.y / 2;
	coord.x = mx;
}


MapChunk* Map::createChunk( signed int x, signed int y )
{
	MapChunk* chunk = NULL;
	ChunkListIter tit;
	tit = get_chunk_iter( x, y );
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
	tit = get_chunk_iter( x, y );
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


ChunkListIter Map::get_chunk_iter( signed int x, signed int y )
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

/*
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
*/

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

void Map::createChunksRectangle( const s2i& pos, const s2i& count )
{
	for( int i = 0; i < count.y; ++i ){
		createHTilesLine( pos.x, pos.y + i , count.x );
	}
}

void Map::deleteChunksRectangle( const s2i& pos, const s2i& count )
{
	for( int i = 0; i < count.x; ++i ){
		deleteVTilesLine( pos.x - i, pos.y - i, count.y );
	}
}


void Map::clear( )
{
	/*
	int cx, cy, ytop;
	if( chunkVec.empty() )
		return;
	cx = static_cast<int>(Camera::getX()) - 64;
	cy = static_cast<int>(Camera::getY()) - 64;
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
	*/
}


void Map::onDraw( )
{
	if( Updated ){
		//TODO: cleaning in thread
		clear();
		Updated = false;
	}
	s3f cam_pos = Camera::position();
	//int cx, cy;
	//cx = static_cast<int>(Camera::getX()) - 64;
	//cy = static_cast<int>(Camera::getY()) - 64;
	s2i chunk_pos( cam_pos.x, cam_pos.y );
	toChunkCoordinates( chunk_pos );
	if( position.x != chunk_pos.x || position.y != chunk_pos.y ){
		createChunksRectangle( chunk_pos, ChunkManager::get_count() );
		position = chunk_pos;
	}
}

