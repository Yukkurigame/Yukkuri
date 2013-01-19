/*
 * config.cpp
 *
 *  Created on: 02.09.2010
 */

#include "config.h"
#include "scripts/LuaConfig.h"
#include "3rdparty/LuaPusher.h"
#include <cstring>
#include <math.h>

MainConfig conf;


bool loadMainConfig( )
{
	LuaConfig* lc = new LuaConfig;
	lc->pushSubconfig( "config_general", "config" );
	lc->get( 1, conf );
	lc->pop( 1 );
	delete lc;


	float right = (float)(conf.mapTileSize >> 2);
	float bottom = (float)(conf.mapTileSize >> 1);
	float tile_side = sqrt( right * right + bottom * bottom );
	conf._tileAngle = asin( right / tile_side );

	return true;
};



// LuaPusher MainConfig
template <>
int pushToLua( lua_State* L, MainConfig* const& val )
{
	// NOT IMPLEMENTED.
	return 0;
}


template<>
MainConfig const getFromLua( lua_State* L, int idx )
{
	MainConfig ret;
	// NOT IMPLEMENTED.
	return ret;
}


template<>
void getFromLua( lua_State* L, int idx, MainConfig& val )
{
	LUA_GET_VALUE( "windows_width", val.windowWidth )
	LUA_GET_VALUE( "windows_height", val.windowHeight )
	LUA_GET_VALUE( "maximum_frame_rate", val.maxFrameRate )
	LUA_GET_VALUE( "minimum_frame_rate", val.minFrameRate )
	LUA_GET_VALUE( "texture_border", val.textureBorder )

	LUA_GET_VALUE( "images_path", val.imagePath )
	LUA_GET_VALUE( "default_image_name", val.defaultImage )
	LUA_GET_VALUE( "fonts_path", val.fontsPath )
	LUA_GET_VALUE( "scripts_path", val.scriptsPath )
	LUA_GET_VALUE( "configs_path", val.configsPath )
	LUA_GET_VALUE( "proto_path", val.protoPath )
	LUA_GET_VALUE( "shaders_path", val.shadersPath )

	LUA_GET_VALUE( "widgets_z", val.widgetsPosZ )

	LUA_GET_VALUE( "map_tile_size", val.mapTileSize )
	LUA_GET_VALUE( "map_default_tile", val.mapDefaultTile )
	LUA_GET_VALUE( "day_length", val.dayLength )
	LUA_GET_VALUE( "action_interval", val.actionInterval )
	LUA_GET_VALUE( "max_spawn", val.maxSpawn )
	LUA_GET_VALUE( "max_edibles", val.maxEdibles )

	LUA_GET_VALUE( "player_dies", val.playerDies )

}

template<>
bool CHINP_TESTER<MainConfig>(lua_State* L, int idx)
{
	if( lua_istable( L, idx ) ){
		lua_pushstring( L, "type" );
		lua_gettable( L, -2 );
		if( lua_isstring( L, 1 ) &&
			!strcmp( "config", lua_tostring( L, 1 ) ) )
			return true;
	}
	return false;
}
