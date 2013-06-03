/*
 * RegionApi.cpp
 *
 *  Created on: 12.12.2012
 */

#include "scripts/api/modules/RegionApi.h"
#include "map/Region.h"
#include "map/Map.h"

#include <string>

int RegionApi::active( lua_State *L )
{
	bool typecheck = lua_isboolean(L, 1) != 0;
	luaL_argcheck( L, typecheck || lua_isnone(L, 1), 1,
			"typecheck lua_isboolean failed" );
	if( typecheck )
		getFromLua< bool >( L, 1 ) ? Map::setActive( ) : Map::clearActive( );
	lua_pop( L, 1 );
	return pushToLua( L, Map::isActive( ) );
}

int RegionApi::generateRegion( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Parameter not given." );
	UINT type = lua_tonumber( L, 1 );
	const char* seed = lua_tostring( L, 2 );
	Region::generate( type, seed );
	lua_pop( L, 2 );
	return 0;
}

int RegionApi::loadRegion( lua_State *L )
{
	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Parameter not given." );
	const char* name = lua_tostring( L, 1 );
	int status = Region::load( name );
	lua_pop( L, 1 );
	lua_pushnumber( L, status );
	return 1;
}

int RegionApi::imageName( lua_State* L )
{
	lua_pushstring( L, Region::get_seed() );
	return 1;
}


int RegionApi::latitude( lua_State* L )
{
	lua_pushnumber( L, Region::get_latitude() );
	return 1;
}


int RegionApi::longitude( lua_State* L )
{
	lua_pushnumber( L, Region::get_longitude() );
	return 1;
}
