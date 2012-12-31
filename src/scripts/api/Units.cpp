/*
 * Units.cpp
 *
 *  Created on: 18.07.2012
 *
 */
#include "units/Unit.h"
#include "units/UnitDynamic.h"
#include "units/unitmanager.h"
#include "scripts/Lua.h"
#include "scripts/LuaScript.h"
#include "scripts/LuaUtils.h"
#include "safestring.h"


int Unit::dist( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ) || lua_isuserdata( L, 1 ), 1,
					"Unit object or id expected" );
	Unit* u = NULL;
	OBJECT_FROM_LUA( 1, UnitManager::GetUnit, Unit*, u )
	return pushToLua( L, dist(u) );
}


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
	CharBuild* p = &Char;
	return p->pushUData( L );
}


int Unit::setAction( lua_State* L )
{
	luaL_argcheck( L, lua_isstring( L, 1 ) || lua_isnumber( L, 1 ), 1, "Action name expected" );

	int action = 0;
	if( lua_isstring( L, 1 ) )
		action = Action::getId( lua_tostring( L, 1 ) );
	else
		action = lua_tointeger( L, 1 );

	bool change = lua_toboolean( L, 2 );

	lua_pop( L, lua_gettop(L) );

	if( !change )
		Actions.saveState( true );
	Actions.setAction( action );

	return 0;
}


int UnitDynamic::addPathTarget( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "X position expected" );
	luaL_argcheck( L, lua_isnumber( L, 2 ), 2, "Y position expected" );
	luaL_argcheck( L, lua_isnone( L, 3 ) || lua_isnumber( L, 3 ), 3, "Number or none expected" );

	float x = lua_tonumber( L, 1 );
	float y = lua_tonumber( L, 2 );
	int crit = lua_tointeger( L, 3 );

	lua_pop( L, lua_gettop(L) );

	addTarget( x, y, crit );

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

	return u->pushUData( L );
}
