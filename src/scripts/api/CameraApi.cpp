/*
 * Camera.cpp
 *
 *  Created on: 23.05.2012
 *
 */

#include "scripts/api/CameraApi.h"
#include "graphics/Camera.h"
#include "units/unitmanager.h"
#include "units/Unit.h"
#include "types.h"


int CameraApi::position(lua_State* L)
{
	lua_newtable( L );
	int top = lua_gettop( L );
	lua_pushstring( L, "x" );
	lua_pushnumber( L, Camera::GetX() );
	lua_settable( L, top );
	lua_pushstring( L, "y" );
	lua_pushnumber( L, Camera::GetY() );
	lua_settable( L, top );
	return 1;
}


int CameraApi::moveTo(lua_State* L)
{
	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Number expected" );
	luaL_argcheck( L, lua_isnumber( L, 2 ), 2, "Number expected" );

	Camera::Move( (float)lua_tonumber(L, 1), (float)lua_tonumber(L, 2) );

	lua_pop( L, 2 );

	return 0;
}


int CameraApi::setMode(lua_State* L)
{
	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Mode expected" );

	Camera::ChangeMode( (enum ctMode)lua_tointeger( L, 1 ) );

	lua_pop( L, 1 );

	return 0;
}


int CameraApi::setTarget(lua_State* L)
{
	Unit* u = NULL;

	luaL_argcheck( L, lua_isnumber( L, 1 ) || lua_isuserdata( L, 1 ) || lua_isnoneornil( L, 1 ),
					1, "Object expected" );

	if( lua_isnumber( L, 1 ) ){
		UINT id = static_cast<UINT>( lua_tointeger(L, 1) );
		u = UnitManager::GetUnit( id );
	}else if( lua_isuserdata( L, 1 ) )
		u = reinterpret_cast<Unit*>( getUserData( L, 1 ) );

	lua_pop( L, 1 );

	Camera::SetTarget( u );

	return 0;
}


int CameraApi::getTarget(lua_State* L)
{
	Unit* u = Camera::GetTarget();
	if( u )
		return u->pushUData(L);
	return 0;
}
