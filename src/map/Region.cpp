/*
 * Region.cpp
 *
 *  Created on: 27.05.2012
 *
 */

#include "map/Region.h"
#include "map/Tiles.h"
#include "scripts/LuaConfig.h"
#include "graphics/Render.h"
#include "graphics/render/Textures.h"
#include "map/generator/MapGen.h"

#include "config.h"
#include "safestring.h"
#include "debug.h"
#include <map>


extern MainConfig conf;


namespace Region {

	std::map< signed int, std::map< signed int, TileInfo* > >  RegionDump;
	TileInfo* tiles;
	bool TilesLoaded = false;
	int TileTypesCount = 0;
	MapGen* mapgen = new MapGen();
	char* seed = NULL;
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
	cfg->getSubconfigsList( config, names );

	int counter = 0;
	ITER_LIST( char*, names ){
		TileInfo& tile = tiles[++counter];
		tile.id = NULL;
		cfg->pushSubconfig( it->data, config );
		cfg->LuaMain::get( -1, tile );
		cfg->pop( 1 );
		free( it->data );

		if( tile.id == conf.mapDefaultTile )
			tiles[0] = tile;
	}

	// One for default tile;
	TileTypesCount++;

	TilesLoaded = true;

	delete cfg;

	return true;
}

void Region::clean( )
{
	delete[] tiles;
}

void Region::generate( UINT form, const char* new_seed )
{
	if( seed )
		free(seed);
	seed = strdup(new_seed);

	mapgen->newIsland( (IslandForm)form, new_seed );
	mapgen->go();
}


void Region::load( const char* name )
{
	Debug::debug( Debug::MAP, "Loading region %s.\n", name );
	if( !TilesLoaded ){
		Debug::debug( Debug::MAP, "Cannot load region %s without tiles.\n", name );
		return;
	}
	LuaConfig* cfg = new LuaConfig;
	list< RegionTile > Tiles;
	cfg->getValue( "tiles", name, "region", Tiles );

	ITER_LIST( RegionTile, Tiles ){
		if( !it->data.id )
			continue;

		signed int x = it->data.postion.x;
		signed int y = it->data.postion.y;
		for( int tile = 1; tile < TileTypesCount; ++tile ){
			if( !strcmp( tiles[tile].id, it->data.id ) ){
				RegionDump[x][y] = &tiles[tile];
				break;
			}
		}
		free( it->data.id );
	}

	delete cfg;
}

const char* Region::get_seed( )
{
	return seed;
}


TileInfo* Region::getTile( signed int x, signed int y )
{
	if( RegionDump.count( x ) > 0) {
		if( RegionDump[x].count( y ) > 0 ){
			return RegionDump[x][y];
		}
	}
	return &tiles[0];
}
