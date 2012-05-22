/*
 * Widgets.cpp
 *
 *  Created on: 20.05.2012
 *
 */


#include "interface/widgets/Widget.h"
#include "interface/widgets/WidgetBar.h"
#include "units/unitmanager.h"

#include "Lua.h"

#include "hacks.h"



bool Widget::toggle( lua_State* L )
{
	this->toggleVisibility();
	return true;
}


bool Widget::bindParam( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ) || lua_isuserdata( L, 1 ), 1, "Object expected" );
	luaL_argcheck( L, lua_isstring( L, 2 ), 2, "Parameter name expected" );

	Unit* u = NULL;
	std::string paramname = "";

	if( lua_isnumber( L, 1 ) ){
		UINT id = static_cast<UINT>( lua_tointeger(L, 1) );
		u = UnitManager::GetUnit( id );
	}else
		u = reinterpret_cast<Unit*>( lua_touserdata( L, 1 ) );

	paramname = lua_tostring( L, 2 );

	lua_pop( L, 2 );

	if( !u ){
		Debug::debug( Debug::INTERFACE, "Bind object not found.\n" );
	}else if( paramname == "" ){
		Debug::debug( Debug::INTERFACE, "Bad bind parameter.\n" );
	}else{
		this->bindValue( u->getUnitpParameter( paramname ) );
		return true;
	}

	return false;
}


LuaRet Widget::getChildren( lua_State* L )
{

	lua_newtable( L );
	int top = lua_gettop( L );
	Widget* child = NULL;
	LuaRet r(1);

	FOREACHIT( this->Children ){
		child = *it;
		lua_pushstring( L, child->getName().c_str() );
		lua_pushinteger( L, child->getId() );
		lua_settable( L, top );
	}
	return r;

}



bool WidgetBar::bindBarMaxValue( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ) || lua_isuserdata( L, 1 ), 1, "Object expected" );
	luaL_argcheck( L, lua_isstring( L, 2 ), 2, "Parameter name expected" );

	Unit* u = NULL;
	std::string paramname = "";

	if( lua_isnumber( L, 1 ) ){
		UINT id = static_cast<UINT>( lua_tointeger(L, 1) );
		u = UnitManager::GetUnit( id );
	}else
		u = reinterpret_cast<Unit*>( lua_touserdata( L, 1 ) );

	paramname = lua_tostring( L, 2 );

	lua_pop( L, 2 );

	if( !u ){
		Debug::debug( Debug::INTERFACE, "Bind object not found.\n" );
	}else if( paramname == "" ){
		Debug::debug( Debug::INTERFACE, "Bad bind parameter.\n" );
	}else{
		this->bindBarMaxValue( u->getUnitpParameter( paramname ) );
		return true;
	}

	return false;
}


