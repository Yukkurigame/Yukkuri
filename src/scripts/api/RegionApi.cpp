/*
 * RegionApi.cpp
 *
 *  Created on: 12.12.2012
 */

#include "scripts/api/RegionApi.h"
#include "map/Region.h"
#include "map/Map.h"

#include <string>


int RegionApi::active( lua_State *L )
{
	bool typecheck = lua_isboolean(L, 1) != 0;
	luaL_argcheck( L, typecheck || lua_isnone(L, 1), 1, "typecheck lua_isboolean failed");
	if(typecheck)
		getFromLua<bool>( L, 1 ) ?  Map::setActive() : Map::clearActive();
	return pushToLua( L, Map::isActive() );
}


int RegionApi::loadRegion( lua_State *L )
{
	std::string name;
	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Parameter not given." );
	name = lua_tostring( L, 1 );
	Region::load( name );
	return 0;
}

