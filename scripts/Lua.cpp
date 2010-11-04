/*
 * Lua.cpp
 *
 *  Created on: 13.07.2010
 */
#include "Lua.h"
#include "debug.h"

LuaStackChecker::LuaStackChecker( lua_State* L, const char* filename/* = ""*/, int line/* = 0*/ )
: luaState_(L),
  filename_(filename),
  line_(line)
{
  top_ = lua_gettop(L);
}

LuaStackChecker::~LuaStackChecker( )
{
	int c;
	c = lua_gettop( luaState_ ) -  top_;
	if( c != 0 ){
		luaL_error( luaState_, "Lua stack corrupted! %d fields. File [%s] line[%d]", c , filename_, line_ );
	}
}


LuaMain::LuaMain()
{
	Lst = luaL_newstate();
    luaL_openlibs(Lst);
}

LuaMain::~LuaMain()
{
	lua_close(Lst);
}

bool LuaMain::OpenFile( string name )
{
	int result;

	result = luaL_loadfile( Lst, name.c_str() );
	if( result ){
		string e = lua_tostring( Lst, -1 );
		debug( 3, "Open " + name + " failed " + e + "\n" );
		return false;
	}

	result = lua_pcall( Lst, 0, LUA_MULTRET, 0);
    if( result ){
    	string e = lua_tostring( Lst, -1 );
    	debug( 3, "Execute " + name + " failed: " + e + "\n" );
    	return false;
    }


    return true;
}


template<> bool LuaMain::getValue( lua_State* L, int index, bool& ret)
{
	ret = false;
	if( lua_isboolean(L, index) ){
		ret = lua_toboolean(L, index) != 0;
		return true;
	}
	return false;
}

//FIXME: Но зачем
template<> bool LuaMain::getValue( lua_State* L, int index, float& ret)
{
	ret = 0.0;
	if( lua_isnumber(L, index) ){
		ret = lua_tonumber(L, index);
		return true;
	}
	return false;
}

template<> bool LuaMain::getValue( lua_State* L, int index, double& ret)
{
	ret = 0.0;
	if( lua_isnumber(L, index) ){
		ret = lua_tonumber(L, index);
		return true;
	}
	return false;
}

template<> bool LuaMain::getValue( lua_State* L, int index, int& ret)
{
	ret = 0;
	if( lua_isnumber(L, index) ){
		ret = static_cast<int>(lua_tonumber(L, index));
		return true;
	}
	return false;
}

template<> bool LuaMain::getValue( lua_State* L, int index, string& ret)
{
	ret = "";
	if( lua_isstring(L, index) ){
		ret = lua_tostring(L, index);
		return true;
	}
	return false;
}
