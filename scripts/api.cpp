/*
 * api.cpp
 *
 *  Created on: 21.09.2010
 */

#include "api.h"
#include "Interface.h"
#include "Luaconfig.h"
#include "unitmanager.h"

int scriptApi::LoadConfig( lua_State* L )
{
	bool result;
	string cname;

	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Config name not given." );

	cname = lua_tostring( L, 1 );
	result = LuaConfig::Instance()->OpenConfig( cname );
	lua_pop( L, lua_gettop( L ) );
	lua_pushboolean( L, result );
	return 1;
}

int scriptApi::LoadAllConfigs( lua_State* L )
{
	bool result;
	string ctype;

	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Configs type not given." );

	ctype = lua_tostring( L, 1 );
	result = LuaConfig::Instance()->LoadAll( ctype );
	lua_pop( L, lua_gettop( L ) );
	lua_pushboolean( L, result );
	return 1;
}

int scriptApi::CreateWidget( lua_State* L )
{
	return 0;
}

int scriptApi::LoadWidget( lua_State* L )
{
	string wname;

	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Widget name not given." );

	wname = lua_tostring( L, 1 );
	Widget* w = UI::yui.LoadWidget( wname );
	lua_pop( L, lua_gettop( L ) );
	if( !w ){
		lua_pushnil( L );
	}else{
		lua_pushinteger( L, w->getId( ) );
	}
	return 1;
}

int scriptApi::BindWidget( lua_State* L )
{
	Widget* w;
	Unit* u;
	string param;
	bool result;

	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Widget id not given." );
	luaL_argcheck( L, lua_isnumber( L, 2 ), 2, "Unit id not given." );
	luaL_argcheck( L, lua_isstring( L, 3 ), 3, "Parameter not given." );


	w = UI::yui.GetWidget( lua_tonumber( L, 1 ) );
	u = UnitManager::units.GetUnit( lua_tonumber( L, 2 ) );
	param = lua_tostring( L, 3 );
	result = false;
	if( w && u && param != "" )
		result = w->bindValue( u->getUnitpParameter( param ) );
	lua_pop( L, lua_gettop( L ) );
	lua_pushboolean( L, result );
	return 1;
}

int scriptApi::WidgetChildren( lua_State* L )
{
	Widget* w;
	int csize;
	int top;

	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Widget id not given." );

	w = UI::yui.GetWidget( lua_tonumber( L, 1 ) );
	lua_pop( L, lua_gettop( L ) );
	if( w ){
		csize = w->childrenCount( );
		Widget* children[csize];
		w->getChildren( children, csize );
		lua_newtable( L );
		top = lua_gettop( L );
		for( int i = 0; i < csize; ++i ){
			lua_pushstring( L, children[i]->getName( ).c_str( ) );
			lua_pushinteger( L, children[i]->getId( ) );
		    lua_settable( L, top );
		}
	}else{
		lua_pushboolean( L, false );
	}
	return 1;
}

int scriptApi::CreateUnit( lua_State* L )
{
	enum unitType type;
	int id;
	float x, y;
	Unit* u;
	luaL_argcheck( L, lua_isnumber(L, 1), 1, "Unit type not given." );

	id = 0;
	type = static_cast<enum unitType>(lua_tonumber( L, 1 ));
	x = lua_tonumber( L, 2 );
	y = lua_tonumber( L, 3 );
	u = UnitManager::units.CreateUnit( type, x, y );
	if( u )
		id = u->getUnitId( );
	lua_pop( L, lua_gettop(L) );
	lua_pushinteger( L, id );

	return 1;
}

int scriptApi::DeleteUnit( lua_State* L )
{
	unsigned int id;
	Unit* u;

	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Unit id not given." );

	id = lua_tonumber( L, 1 );
	u = UnitManager::units.GetUnit( id );
	lua_pop( L, lua_gettop( L ) );
	if( !id || !u ){
		lua_pushboolean( L, false );
		return 1;
	}
	UnitManager::units.DeleteUnit( u );
	lua_pushboolean( L, true );

	return 1;
}
