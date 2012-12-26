/*
 * YOBA.cpp
 *
 *  Created on: 26.12.2012
 */

#include "units/YOBA.h"
#include "scripts/Lua.h"



float CharBuild::get( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Parameter name excepted" );
	return get( lua_tonumber( L, 1 ) );
}


float CharBuild::set( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Parameter name excepted" );
	luaL_argcheck( L, lua_isnumber( L, 2 ), 2, "Parameter value excepted" );
	int param = lua_tonumber( L, 1 );
	set( param, lua_tonumber( L, 2 ) );
	return get(param);
}


int CharBuild::tire( lua_State* L )
{
	luaL_argcheck( L, lua_isnil( L, 1 ) || lua_isnumber( L, 1 ), 1, "Value or nil excepted" );
	tire( lua_tonumber( L, 1 ) );
	return 0;
}


int CharBuild::levelUp( lua_State* L )
{
	luaL_argcheck( L, lua_isnil( L, 1 ) || lua_isnumber( L, 1 ), 1, "Value or nil excepted" );
	int l = lua_tonumber( L, 1 );
	levelUp( l ? l : 1 );
	return level;
}


