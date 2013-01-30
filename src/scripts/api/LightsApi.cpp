/*
 * LightsApi.cpp
 *
 *  Created on: 30.01.2013
 *
 */

#include "scripts/api/LightsApi.h"
#include "graphics/Lighting.h"


int LightsApi::create( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Light type not given." );

	enum LightType type = static_cast<enum LightType>(lua_tonumber( L, 1 ));

	lua_pop( L, lua_gettop( L ) );

	LightSource* l = LightingManager::add_light( type );

	if( !l )
		lua_pushnil( L );
	else
		l->pushUData( L );

	return 1;
}




int LightsApi::remove( lua_State* L )
{
	return 0;
}


int LightsApi::count( lua_State* L )
{
	return 0;
}
