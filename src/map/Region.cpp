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

#include "config.h"
#include "safestring.h"
#include "debug.h"

extern MainConfig conf;


namespace {

	std::map< signed int, std::map< signed int, TileInfo* > >  RegionDump;
	TileInfo* tiles;
	bool TilesLoaded = false;
	unsigned int TileTypesCount = 0;
}


bool Region::init( )
{
	if(TilesLoaded){
		Debug::debug( Debug::MAP, "Tiles already loaded." );
		return false;
	}
	LuaConfig* cfg = new LuaConfig;
	//FIXME: И тут, внезапно, в функцию врываются костыли.
	std::vector <  std::map < std::string, std::string > > Subconfigs;
	if( !cfg->getSubconfigs( "tile", Subconfigs ) || ! Subconfigs.size() ){
		Debug::debug( Debug::CONFIG, "Tiles configs opening error or no tiles found.\n");
		return false;
	}else{
		TileTypesCount = Subconfigs.size();
		Debug::debug( Debug::MAP, "Tiles found: " + citoa(TileTypesCount) + ".\n" );
	}
	tiles = new TileInfo[TileTypesCount + 1];
	for( unsigned int i = 0; i < TileTypesCount; ++i ){
		std::map < std::string, std::string >& Subconfig = Subconfigs[i];
		TileInfo& tile = tiles[i + 1];
		if( Subconfig.count("id") < 1 ){
			Debug::debug( Debug::MAP, "Tile have no id.\n" );
			continue;
		}
		tile.id = Subconfig["id"];
		if( Subconfig.count("image") ){
			std::string image = Subconfig["image"];
			tile.texture = Textures::get_by_name( image.c_str() );
		}
		tile.picture = Subconfig.count("picture") ? atoi(Subconfig["picture"].c_str()) : 0;
		// FIXME: useless
		tile.passability = (float)(Subconfig.count("passability") ? atoi(Subconfig["passability"].c_str()) : 0);
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

void Region::load( std::string name )
{
	Debug::debug( Debug::MAP, "Loading region " + name + ".\n" );
	if( !TilesLoaded ){
		Debug::debug( Debug::MAP, "Cannot load region " + name + " without tiles.\n");
		return;
	}
	LuaConfig* cfg = new LuaConfig;
	std::vector< std::map< std::string, std::string > > Tiles;
	cfg->getValue( "tiles", name.c_str(), "region", Tiles );
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
			for( unsigned int tile = 1; tile < TileTypesCount; ++tile ){
				if( tiles[tile].id == type ){
					RegionDump[x][y] = &tiles[tile];
					break;
				}
			}
		}
	}
	delete cfg;
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
