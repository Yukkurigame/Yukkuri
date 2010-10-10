/*
 * LuaScript.cpp
 *
 *  Created on: 21.09.2010
 */

#include "LuaScript.h"
#include "api.h"
#include "config.h"

LuaScript::LuaScript( )
{

}

bool LuaScript::Init()
{
	RegisterApi( );
	return true;
}

void LuaScript::RegisterApi( )
{
	lua_register( Lst, "LoadConfig", &scriptApi::LoadConfig );
	lua_register( Lst, "LoadAllConfigs", &scriptApi::LoadAllConfigs );

	lua_register( Lst, "CreateWidget", &scriptApi::CreateWidget );
	lua_register( Lst, "LoadWidget", &scriptApi::LoadWidget );
	lua_register( Lst, "BindWidget", &scriptApi::BindWidget );
	lua_register( Lst, "WidgetChildren", &scriptApi::WidgetChildren );

	lua_register( Lst, "CreateUnit", &scriptApi::CreateUnit );
	lua_register( Lst, "DeleteUnit", &scriptApi::DeleteUnit );
}

bool LuaScript::OpenFile( string name )
{
	extern MainConfig conf;
	return LuaMain::OpenFile( ( ( conf.scriptsPath != "" ) ? conf.scriptsPath : "data/scripts/" ) + name + ".lua" );
}
