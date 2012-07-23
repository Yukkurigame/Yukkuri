/*
 * Interface.cpp
 *
 *  Created on: 20.05.2012
 *
 */

#include "interface/Interface.h"
#include "scripts/api/InterfaceApi.h"

#include "types.h"
#include "safestring.h"


int IfaceApi::loadAllWidgets( lua_State* L )
{
	return 0;
}


int IfaceApi::loadWidget( lua_State* L )
{
	std::string wname;

	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Widget name not given." );

	wname = lua_tostring( L, 1 );
	Widget* w = Interface::LoadWidget( wname );
	lua_pop( L, lua_gettop( L ) );
	if( !w ){
		lua_pushnil( L );
	}else{
		lua_pushinteger( L, w->getId( ) );
	}
	return 1;
}


int IfaceApi::getWidget( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Widget id expected" );

	UINT id = static_cast<UINT>( lua_tointeger(L, 1) );
	Widget* w = Interface::GetWidget( id );
	if( w )
		return w->pushUData(L);

	Debug::debug( Debug::UNIT, "getUnit: unit with id " + citoa(id) + " does not exist.\n" );
	return 0;
}

int IfaceApi::getWidgetByName( lua_State* L )
{
	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Name expected" );
	luaL_argcheck( L, lua_isnumber( L, 2 ) || lua_isuserdata( L, 2 ) || \
			lua_isnoneornil( L, 2 ), 2, "Parent widget or nil expected" );

	std::string wname;
	Widget* parent = NULL;

	wname = lua_tostring( L, 1 );

	if( lua_isnumber( L, 2 ) ){
		UINT id = static_cast<UINT>( lua_tointeger(L, 2) );
		parent = Interface::GetWidget( id );
	}else if( lua_isuserdata( L, 2 ) )
		parent = reinterpret_cast<Widget*>( lua_touserdata( L, 2 ) );

	Widget* w = Interface::GetWidget( wname, parent );
	if( w )
		return w->pushUData(L);

	return 0;
}

