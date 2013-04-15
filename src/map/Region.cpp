/*
 * Region.cpp
 *
 *  Created on: 27.05.2012
 *
 */

#include "map/Region.h"
#include "map/Tiles.h"
#include "map/generator/MapGenerator.h"
#include "map/Packer.h"
#include "scripts/LuaConfig.h"

#include "config.h"
#include "safestring.h"
#include "debug.h"

//#include <map>


extern MainConfig conf;


namespace Region {

	//std::map< signed int, std::map< signed int, TileInfo* > >  RegionDump;
	TileInfo* tiles;
	bool TilesLoaded = false;
	int TileTypesCount = 0;

	RegionMap* active_region = NULL;
	char* active_path = NULL;
}



bool Region::init( )
{
	if(TilesLoaded){
		Debug::debug( Debug::MAP, "Tiles already loaded." );
		return false;
	}

	LuaConfig* cfg = new LuaConfig;
	const char* config = "tile";
	list< char* > names;

	cfg->getSubconfigsLength( config, TileTypesCount );

	if( !TileTypesCount ){
		Debug::debug( Debug::CONFIG, "Tiles configs opening error or no tiles found.\n");
		return false;
	}else{
		Debug::debug( Debug::MAP, "Tiles found: %d.\n", TileTypesCount );
	}

	tiles = new TileInfo[TileTypesCount + 1];
	tiles[0].id = NULL;
	cfg->getSubconfigsList( config, names );

	int counter = 0;
	ITER_LIST( char*, names ){
		TileInfo& tile = tiles[++counter];
		tile.id = NULL;
		cfg->pushSubconfig( it->data, config );
		cfg->LuaMain::get( -1, tile );
		cfg->pop( 1 );
		free( it->data );

		if( !strcmp(tile.id, conf.mapDefaultTile.c_str()) )
			tiles[0] = tile;
	}

	// One for default tile;
	TileTypesCount++;

	TilesLoaded = true;

	delete cfg;

	MapGenerator::init( );

	return true;
}

void Region::clean( )
{
	for( int i = 1; i < TileTypesCount; ++i ){
		free( tiles[i].id );
		tiles[i].id = NULL;
	}

	delete[] tiles;

	MapGenerator::clean();
}

void Region::generate( UINT form, const char* new_seed )
{
	MapGenerator::newIsland( (IslandForm)form, new_seed );
	MapGenerator::go();
}


int Region::load( const char* path )
{
	Debug::debug( Debug::MAP, "Loading region %s.\n", path );
	if( !TilesLoaded ){
		Debug::debug( Debug::MAP, "Cannot load region without tiles.\n" );
		return 0;
	}

	unload();

	active_region = new RegionMap;
	if( !MapPacker::unpack_region( path, active_region ) ){
		Debug::debug( Debug::MAP, "Cannot unpack region in %s.\n", path );
		delete active_region, active_region = NULL;
		return 0;
	}

	active_path = strdup(path);

	return 1;
}

void Region::unload( )
{
	if( active_path ){
		free(active_path);
		active_path = NULL;
	}

	if( !active_region )
		return;

	Debug::debug( Debug::MAP, "Unloading region.\n" );
	delete active_region, active_region = NULL;
}

float Region::get_latitude( )
{
	if( !active_region )
		return 0.0;
	return active_region->latitude;
}


float Region::get_longitude( )
{
	if( !active_region )
		return 0.0;
	return active_region->longitude;
}


const char* Region::get_seed( )
{
	if( !active_region )
		return NULL;
	return active_region->seed_string;
}


TileInfo* Region::getTile( signed int x, signed int y )
{
	/*if( RegionDump.count( x ) > 0) {
		if( RegionDump[x].count( y ) > 0 ){
			return RegionDump[x][y];
		}
	}
	return &tiles[0];
	*/
	return NULL;
}


ChunkTile* Region::getChunk( signed int x, signed int y )
{
	ChunkTile* chunk = new ChunkTile;
	if( !MapPacker::unpack_chunk( active_path, x, y, chunk ) ){
		Debug::debug( Debug::MAP, "Cannot unpack chunk %d:%d in %s.\n", x, y, active_path );
		delete chunk, chunk = NULL;
		return NULL;
	}

	return chunk;
}
