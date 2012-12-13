/*
 * Widgets.cpp
 *
 *  Created on: 20.05.2012
 *
 */


#include "interface/widgets/Widget.h"
#include "interface/widgets/WidgetBar.h"
#include "units/unitmanager.h"
#include "basic_types.h"

#include "scripts/Lua.h"

#include "hacks.h"

extern float currentFPS;


bool Widget::load( lua_State* L )
{
	return true;
}


bool Widget::resize( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Width expected" );
	luaL_argcheck( L, lua_isnumber( L, 2 ), 2, "Height expected" );

	this->resize( (float)lua_tonumber( L, 1 ), (float)lua_tonumber( L, 2 ) );

	lua_pop( L, 2 );

	return true;
}

bool Widget::toggle( lua_State* L )
{
	this->toggleVisibility();
	return this->visible;
}


bool Widget::bindParam( lua_State* L )
{
	luaL_argcheck( L, lua_isnil( L, 1 ) || lua_isnumber( L, 1 ) || lua_isuserdata( L, 1 ), 1, "Object expected" );
	luaL_argcheck( L, lua_isnumber( L, 2 ), 2, "Parameter name expected" );

	Unit* u = NULL;
	bool core = false;
	int paramname;

	if( lua_isnil( L, 1 ) ){
		core = true;
	}else if( lua_isnumber( L, 1 ) ){
		UINT id = static_cast<UINT>( lua_tointeger(L, 1) );
		u = UnitManager::GetUnit( id );
	}else
		u = reinterpret_cast<Unit*>( lua_touserdata( L, 1 ) );

	paramname = lua_tointeger( L, 2 );

	lua_pop( L, 2 );

	if( paramname <= 0 ){
		Debug::debug( Debug::INTERFACE, "Bad bind parameter.\n" );
		return false;
	}

	if( core ){
		switch(paramname){
			case gbpFPS:
				this->bindValue( tiFloat, &(currentFPS) );
				break;
			default:
				Debug::debug( Debug::INTERFACE, "Bad bind parameter.\n" );
				break;
		}
		return true;
	}

	if( !u ){
		Debug::debug( Debug::INTERFACE, "Bind object not found.\n" );
	}else{
		void* param = NULL;
		enum type_identifier type = tiNone;
		// FIXME: binded values point to wrong memory when unit dies.
		if( paramname < uCharIntLast ){
			param = u->getUnitpParameter( (enum character)paramname );
			type = tiInt;
		}else{
			param = u->getUnitpParameter( (enum character_float)paramname );
			type = tiFloat;
		}
		this->bindValue( type, param );
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

	listElement<Widget*>* l = Children.head;
	while( l != NULL ){
		child = l->data;
		lua_pushstring( L, child->getWidgetName().c_str() );
		lua_pushinteger( L, child->getWidgetId() );
		lua_settable( L, top );
		l = l->next;
	}
	return r;

}


bool WidgetBar::bindBarMaxValue( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ) || lua_isuserdata( L, 1 ), 1, "Object expected" );
	luaL_argcheck( L, lua_isstring( L, 2 ), 2, "Parameter name expected" );

	Unit* u = NULL;
	enum character_float paramname;

	if( lua_isnumber( L, 1 ) ){
		UINT id = static_cast<UINT>( lua_tointeger(L, 1) );
		u = UnitManager::GetUnit( id );
	}else
		u = reinterpret_cast<Unit*>( lua_touserdata( L, 1 ) );

	paramname = static_cast<enum character_float>( lua_tointeger( L, 2 ) );

	lua_pop( L, 2 );

	if( !u ){
		Debug::debug( Debug::INTERFACE, "Bind object not found.\n" );
	}else if( paramname == 0 ){
		Debug::debug( Debug::INTERFACE, "Bad bind parameter.\n" );
	}else{
		this->bindBarMaxValue( u->getUnitpParameter( paramname ) );
		return true;
	}

	return false;
}


