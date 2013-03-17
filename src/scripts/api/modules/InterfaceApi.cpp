/*
 * Interface.cpp
 *
 *  Created on: 20.05.2012
 *
 */

#include "interface/Interface.h"
#include "interface/widgets/Widget.h"
#include "scripts/api/modules/InterfaceApi.h"
#include "scripts/LuaUtils.h"

#include "types.h"
#include "safestring.h"


int IfaceApi::createWidget( lua_State* L )
{
	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Widget name not given." );
	luaL_argcheck( L, lua_isnumber( L, 2 ), 2, "Widget type not given." );

	const char* wname = lua_tostring( L, 1 );
	enum wType type = (enum wType)lua_tointeger( L, 2 );
	Widget* w = Interface::CreateWidget( wname, type );

	if( lua_istable( L, 3 ) ){
		// Move table to first position
		lua_pushvalue( L, 3 );
		lua_insert( L, 1 );
		if( !w->load( L ) )
			Debug::debug( Debug::INTERFACE,
					"Cannot load widget parameters for widget %s.\n", w->getWidgetName().c_str() );
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

	Debug::debug( Debug::UNIT, "getUnit: unit with id %d does not exist.\n", id );
	return 0;
}


int IfaceApi::getWidgetByName( lua_State* L )
{
	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Name expected" );
	luaL_argcheck( L, lua_isnumber( L, 2 ) || lua_isuserdata( L, 2 ) || \
			lua_isnoneornil( L, 2 ), 2, "Parent widget or nil expected" );


	const char* wname = lua_tostring( L, 1 );
	Widget* parent = NULL;

	OBJECT_FROM_LUA( 2, Interface::GetWidget, Widget*, parent )

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

