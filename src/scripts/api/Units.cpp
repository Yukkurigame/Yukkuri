/*
 * Units.cpp
 *
 *  Created on: 18.07.2012
 *
 */
#include "units/Unit.h"
#include "scripts/Lua.h"

int Unit::color( lua_State* L )
{
	// TODO: rewrite

	int top = lua_gettop(L);
	/*
	color4u& clr = Image.getSprite()->clr;

	if( top == 0 ){
		lua_newtable(L);
		lua_pushinteger( L, clr.r );
		lua_rawseti( L, -2, 1 );
		lua_pushinteger( L, clr.g );
		lua_rawseti( L, -2, 2 );
		lua_pushinteger( L, clr.b );
		lua_rawseti( L, -2, 3 );
		lua_pushinteger( L, clr.a );
		lua_rawseti( L, -2, 4 );
	}else if( top > 2 && top < 5 ){
		getFromLua( L, -top, clr.r );
		getFromLua( L, 1 - top, clr.g );
		getFromLua( L, 2 - top, clr.b );
		if( lua_isnumber( L, 4) )
			getFromLua( L, 3 - top, clr.a );
	}else{
		Debug::debug( Debug::SCRIPT, "Color: bad arguments count" );
	}
	*/

	lua_pop( L, top );

	return top ? 0 : 1;
}


