/*
 * Interface.cpp
 *
 *  Created on: 20.05.2012
 *
 */

#include "interface/Interface.h"
#include "scripts/api/InterfaceApi.h"
#include "scripts/LuaUtils.h"

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
		lua_pushinteger( L, w->getWidgetId( ) );
	}
	return 1;
}


int IfaceApi::createWidget( lua_State* L )
{
	std::string wname;
	enum wType type;

	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Widget name not given." );
	luaL_argcheck( L, lua_isnumber( L, 2 ), 2, "Widget type not given." );

	wname = lua_tostring( L, 1 );
	type = (enum wType)lua_tointeger( L, 2 );
	Widget* w = Interface::CreateWidget( wname, type );

	if( lua_istable( L, 3 ) ){
		// Move table to first position
		lua_pushvalue( L, 3 );
		lua_insert( L, 1 );
		if( !w->load( L ) )
			Debug::debug( Debug::INTERFACE, "Cannot load widget parameters for widget " + w->getWidgetName() + ".\n" );
		// TODO: stack order changed
	}

	lua_pop( L, lua_gettop( L ) );
	if( !w )
		lua_pushnil( L );
	else
		return w->pushUData(L);
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
		parent = reinterpret_cast<Widget*>( getUserData( L, 2 ) );

	Widget* w = Interface::GetWidget( wname, parent );
	if( w )
		return w->pushUData(L);

	return 0;
}


int IfaceApi::addChild( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ) || lua_isuserdata( L, 1 ), 1,
						"Widget object or id expected" );
	luaL_argcheck( L, lua_isnumber( L, 2 ) || lua_isuserdata( L, 2 ) || \
					( lua_isstring( L, 2 ) && lua_isnumber( L, 3 ) ), 2,
					"Widget object or widget name and type expected" );

	// Stack: parent_widget, children || children_id (, children_params )

	Widget* parent = NULL;
	Widget* children = NULL;

	OBJECT_FROM_LUA( 1, Interface::GetWidget, Widget*, parent )
	/*
	if( lua_isnumber( L, 1 ) ){
		UINT id = static_cast<UINT>( lua_tointeger(L, 1) );
		parent = Interface::GetWidget( id );
	}else if( lua_isuserdata( L, 1 ) )
		parent = reinterpret_cast<Widget*>( getUserData( L, 1 ) );
	*/

	if( !parent )
		return 0;

	// Remove first argument from stack
	lua_remove( L, 1 );
	// Stack: children || children_id (, children_params )

	if( lua_isnumber( L, 1 ) ){
		UINT id = static_cast<UINT>( lua_tointeger(L, 1) );
		children = Interface::GetWidget( id );
	}else{
		if( lua_isstring( L, 1 ) && lua_isnumber( L, 2 ) )
			createWidget( L );
		// Stack: children_userdata
		if( lua_isuserdata( L, 1 ) )
			children = reinterpret_cast<Widget*>( getUserData( L, 1 ) );
	}

	parent->addChild(children);

	// Stack: children
	return 1;
}

