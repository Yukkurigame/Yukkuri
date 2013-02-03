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
void getFromLua( lua_State* L, int idx, MainConfig& val )
{
	lua_pushstring( L, "video" );	/* st: table string	*/	\
	lua_gettable( L, -2 );			/* st: table val	*/	\
	LUA_GET_VALUE( "window_width", val.video.windowWidth )
	LUA_GET_VALUE( "window_height", val.video.windowHeight )
	LUA_GET_VALUE( "maximum_frame_rate", val.video.maxFrameRate )
	LUA_GET_VALUE( "minimum_frame_rate", val.video.minFrameRate )
	LUA_GET_VALUE( "render_method", val.video.renderMethod )
	lua_pop( L, 1 );

	lua_pushstring( L, "paths" );	/* st: table string	*/	\
	lua_gettable( L, -2 );			/* st: table val	*/	\
	LUA_GET_VALUE( "images", val.path.image )
	LUA_GET_VALUE( "normals", val.path.normals )
	LUA_GET_VALUE( "fonts", val.path.fonts )
	LUA_GET_VALUE( "scripts", val.path.scripts )
	LUA_GET_VALUE( "configs", val.path.configs )
	LUA_GET_VALUE( "proto", val.path.proto )
	LUA_GET_VALUE( "shaders", val.path.shaders )
	LUA_GET_VALUE( "meshes", val.path.meshes )
	lua_pop( L, 1 );

	LUA_GET_VALUE( "default_image_name", val.default_image )

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
