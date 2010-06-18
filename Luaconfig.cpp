/*
 * Luaconfig.cpp
 *
 *  Created on: 04.06.2010
 */

#include "Luaconfig.h"

LuaConfig::LuaConfig( )
{
	Lconf = lua_open();
    luaL_openlibs(Lconf);
    luaL_loadfile(Lconf, "data/scripts/yamlconf.lua"); //TODO add SCRIPTPATH
    lua_pcall( Lconf, 0, LUA_MULTRET, 0 );
}

bool LuaConfig::OpenConfig( std::string filename )
{
	int stat;
	//Load config file from yaml
	lua_getfield( Lconf, LUA_GLOBALSINDEX, "load" );
	lua_pushstring( Lconf, filename.c_str() );
	stat = lua_pcall( Lconf, 1, 0, 0 );
	if( stat )
			return false;
	return true;
}

LuaConfig::~LuaConfig( )
{
	lua_close(Lconf);
}

//TODO: same functions
double LuaConfig::getNumber( string field )
{
	double ret;
	lua_getfield( Lconf, LUA_GLOBALSINDEX, "get");
	lua_pushstring( Lconf, field.c_str() );
	lua_call( Lconf, 1, 1 );
	ret = lua_tonumber( Lconf, -1 );
	return ret;
}

string LuaConfig::getString( string field )
{
    string ret;
	lua_getfield( Lconf, LUA_GLOBALSINDEX, "get");
	lua_pushstring( Lconf, field.c_str() );
	lua_call( Lconf, 1, 1 );
	ret = lua_tostring( Lconf, -1 );
	return ret;
}
