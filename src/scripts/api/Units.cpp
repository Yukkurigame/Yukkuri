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
#include "safestring.h"


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
	}else if( top > 2 && top < 5 ){
		for( int i=0; i<4; ++i ){
			if( lua_isnumber( L, i+1 ) )
				getFromLua( L, i - top, *p[i] );
		}
		brush.set_color( clr );
	}else{
		Debug::debug( Debug::SCRIPT, "Color: bad arguments count" );
	}

	lua_pop( L, top );

	return top ? 0 : 1;
}


int Unit::emitEvent( lua_State* L )
{
	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Event name expected" );

	// Excess call for debug purposes.
	std::string name = lua_tostring( L, 1 );

	int top = lua_gettop( L ); 							// stack: event_name, params ...,

	extern LuaScript* luaScript;

	int argcount = 1;
	if( events.self != LUA_NOREF ){
		luaScript->GetFromRegistry( L, events.self );	// stack: event_name, params ..., events_table
		lua_insert( L, argcount );						// stack: events_table, event_name, params ...
		++argcount;
	}
	this->pushUData( L );			// stack: event_name, params ..., events_table, udata
	lua_insert( L, argcount );		// stack: events_table, udata, event_name, params ...

	int ret_val = luaScript->ExecChunkFromReg( events.function, argcount + top );
	if( ret_val == -1 )
		Debug::debug( Debug::PROTO,
			"An error occurred while executing a local event function '" + name +
			"'. obj id  " + citoa(UnitId) + ", proto_name '" + Actions.proto->name +
			"', action '" + Action::getName(Actions.action->id)  + "', frame " + citoa(Actions.frame) +
			": " + luaScript->getString( -1 ) + ".\n" );

	return ret_val;
}


int Unit::getBuild( lua_State* L )
{
	Char.pushUData( L );
	return 1;
}


int Unit::setAction( lua_State* L )
{
	luaL_argcheck( L, lua_isstring( L, 1 ) || lua_isnumber( L, 1 ), 1, "Action name expected" );

	int action = 0;
	if( lua_isstring( L, 1 ) )
		action = Action::getId( lua_tostring( L, 1 ) );
	else
		action = lua_tointeger( L, 1 );
	lua_pop( L, 1 );

	Actions.setAction( action );

	return 0;
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
