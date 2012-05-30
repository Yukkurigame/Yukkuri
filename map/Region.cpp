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

#include "config.h"
#include "safestring.h"
#include "debug.h"

extern MainConfig conf;


namespace {

	std::map< signed int, std::map< signed int, TileInfo* > >  RegionDump;
	std::map< std::string, TileInfo > tiles;
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
	for( unsigned int i = 0; i < TileTypesCount; ++i ){
		if( Subconfigs[i].count("id") < 1 ){
			Debug::debug( Debug::MAP, "Tile have no id.\n" );
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


void Region::load( std::string name )
{
	Debug::debug( Debug::MAP, "Loading region " + name + ".\n" );
	if( !TilesLoaded ){
		Debug::debug( Debug::MAP, "Cannot load region " + name + " without tiles.\n");
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


TileInfo* Region::getTile( signed int x, signed int y )
{
	if( RegionDump.count( x ) > 0) {
		if( RegionDump[x].count( y ) > 0 ){
			return RegionDump[x][y];
		}
	}
	return &tiles[conf.mapDefaultTile];
}
