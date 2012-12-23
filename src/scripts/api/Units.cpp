/*
 * Units.cpp
 *
 *  Created on: 18.07.2012
 *
 */
#include "units/Unit.h"
#include "units/UnitDynamic.h"
#include "scripts/Lua.h"
#include "scripts/LuaScript.h"

extern LuaScript* luaScript;


int Unit::color( lua_State* L )
{
	int top = lua_gettop(L);
	GLBrush& brush = Image.getSprite()->brush;
	s4ub clr = brush.get_color();
	GLubyte* p[4] = { &clr.r, &clr.g, &clr.b, &clr.a };

	if( top == 0 ){
		lua_newtable(L);
		for( int i=0; i<4; ++i ){
			lua_pushinteger( L, *p[i] );
			lua_rawseti( L, -2, i+1 );
		}
		/*
		lua_pushinteger( L, clr.r );
		lua_rawseti( L, -2, 1 );
		lua_pushinteger( L, clr.g );
		lua_rawseti( L, -2, 2 );
		lua_pushinteger( L, clr.b );
		lua_rawseti( L, -2, 3 );
		lua_pushinteger( L, clr.a );
		lua_rawseti( L, -2, 4 );
		*/
	}else if( top > 2 && top < 5 ){
		for( int i=0; i<4; ++i ){
			if( lua_isnumber( L, i+1 ) )
				getFromLua( L, i - top, *p[i] );
		}
		/*getFromLua( L, -top, clr.r );
		getFromLua( L, 1 - top, clr.g );
		getFromLua( L, 2 - top, clr.b );
		if( lua_isnumber( L, 4) )
			getFromLua( L, 3 - top, clr.a );
		*/
		brush.set_color( clr );
	}else{
		Debug::debug( Debug::SCRIPT, "Color: bad arguments count" );
	}

	lua_pop( L, top );

	return top ? 0 : 1;
}


int UnitDynamic::getClosest( lua_State* L )
{
	int types;
	float limit;
	int top = lua_gettop(L);
	getFromLua( L, 1, types );
	getFromLua( L, 2, limit );
	lua_pop( L, top );

	Unit* u = scope.closest( types ? types : utAll, limit ? limit : 100.0 );

	if( !u )
		return 0;

	u->pushUData( L );

	return 1;
}
