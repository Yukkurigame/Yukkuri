/*
 * Bindings.cpp
 *
 *  Created on: 24.06.2012
 *
 */

#include "scripts/api/modules/BindingsApi.h"
#include "scripts/LuaScript.h"
#include "Bindings.h"

extern LuaScript* luaScript;

int BindingsApi::set( lua_State* L )
{
	const char* bname;

	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Bindings id not given." );

	bname = lua_tostring( L, 1 );
	lua_pop( L, 1 );

	Bindings::loadKeys( bname );


	return 0;
}


int BindingsApi::getName( lua_State* L )
{
	lua_pushstring( L, Bindings::getCurrent() );

	return 1;
}

int BindingsApi::setReciever( lua_State* L )
{
	LuaRegRef& ref = Bindings::getReciever();

	luaScript->RegProc( L, &ref, 1 );

	return 0;
}
