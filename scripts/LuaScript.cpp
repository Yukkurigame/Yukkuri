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
	lua_register( Lst, "Debug", &scriptApi::Debug );

	lua_register( Lst, "LoadConfig", &scriptApi::LoadConfig );
	lua_register( Lst, "LoadAllConfigs", &scriptApi::LoadAllConfigs );

	lua_register( Lst, "CreateWidget", &scriptApi::CreateWidget );
	lua_register( Lst, "LoadWidget", &scriptApi::LoadWidget );
	lua_register( Lst, "BindWidget", &scriptApi::BindWidget );
	lua_register( Lst, "BindWidgetMaxBar", &scriptApi::BindWidgetMaxBar );
	lua_register( Lst, "WidgetChildren", &scriptApi::WidgetChildren );
	lua_register( Lst, "GetWidgetName", &scriptApi::GetWidgetName );
	lua_register( Lst, "WidgetSetBarSize", &scriptApi::WidgetSetBarSize );

	lua_register( Lst, "CreateUnit", &scriptApi::CreateUnit );
	lua_register( Lst, "DeleteUnit", &scriptApi::DeleteUnit );

	lua_register( Lst, "NewThread", &scriptApi::NewThread );
	lua_register( Lst, "ThreadWait", &scriptApi::ThreadWait );
	lua_register( Lst, "ResumeThread", &scriptApi::ResumeThread );
	lua_register( Lst, "RemoveThread", &scriptApi::RemoveThread );
}

bool LuaScript::OpenFile( string name )
{
	extern MainConfig conf;
	return LuaMain::OpenFile( ( ( conf.scriptsPath != "" ) ? conf.scriptsPath : "data/scripts/" ) + name + ".lua" );
}
