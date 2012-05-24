/*
 * UnitManager.cpp
 *
 *  Created on: 19.05.2012
 *
 */
#include "scripts/api/UnitManager.h"
#include "units/unitmanager.h"

#include "safestring.h"
#include "debug.h"


//UnitManager Lua interface
int UMApi::createUnit( lua_State* L )
{
	unitType type;
	int x, y;
	luaL_argcheck( L, lua_isnumber(L, 1), 1, "Unit type excepted." );
	luaL_argcheck( L, lua_isnumber(L, 2) || lua_isnoneornil(L, 2), 2, "Number or nil excepted." );
	luaL_argcheck( L, lua_isnumber(L, 3) || lua_isnoneornil(L, 3), 3, "Number or nil excepted." );

	x = y = 0;
	type = (unitType)lua_tonumber(L, 1);
	x = lua_tonumber(L, 2);
	y = lua_tonumber(L, 3);

	lua_pop( L, lua_gettop( L ) );

	Unit* ret = UnitManager::CreateUnit( type, x, y );
	if( !ret ){
		Debug::debug( Debug::UNIT, "Unit creation failed.\n" );
		lua_pushnil( L );
	}else{
		lua_pushnumber( L, ret->getUnitId() );
	}

	return 1;
}


int UMApi::deleteUnit( lua_State* L )
{
	Unit* u = NULL;

	luaL_argcheck( L, lua_isnumber( L, 1 ) || lua_isuserdata( L, 1 ), 1, "Unit expected" );

	if( lua_isnumber( L, 1 ) ){
		UINT id = static_cast<UINT>( lua_tointeger(L, 1) );
		u = UnitManager::GetUnit( id );
	}else
		u = reinterpret_cast<Unit*>( lua_touserdata( L, 1 ) );

	lua_pop( L, 1 );

	if( u == NULL ){
		lua_pushboolean( L, false );
		Debug::debug( Debug::UNIT, "deleteUnit: unit not found.\n" );
	}else{
		UnitManager::RemoveUnit( u );
		lua_pushboolean( L, true );
	}

	return 1;
}


int UMApi::getUnit( lua_State* L )
{

	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Unit id expected" );

	UINT id = static_cast<UINT>( lua_tointeger(L, 1) );
	Unit* u = UnitManager::GetUnit( id );
	if( u )
		return u->pushUData(L);

	Debug::debug( Debug::UNIT, "getUnit: unit with id " + citoa(id) + " does not exist.\n" );
	return 0;

}


int UMApi::getPlayer( lua_State* L )
{

	Unit* player = UnitManager::GetPlayer();
	if( player )
		return player->pushUData(L);

	Debug::debug( Debug::UNIT, "getPlayer: player does not exists.\n" );
	return 0;
}


int UMApi::getUnitsSize( lua_State* L )
{
	int count = 0;
	unitType type;
	luaL_argcheck(L, lua_isnumber(L, 1) || lua_isnoneornil(L, 1), 1, "Units type or nil excepted.");

	if( lua_isnoneornil(L, 1) )
		count = UnitManager::GetUnitVecSize();
	else{
		type = (unitType)lua_tonumber(L, 1);
		count = UnitManager::GetUnitsSize( type );
	}

	lua_pop( L, lua_gettop( L ) );
	lua_pushinteger( L, count );

	return 1;
}

