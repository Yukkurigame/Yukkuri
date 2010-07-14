/*
 * Lua.cpp
 *
 *  Created on: 13.07.2010
 */
#include "Lua.h"

LuaStackChecker::LuaStackChecker( lua_State* L, const char* filename/* = ""*/, int line/* = 0*/ )
: luaState_(L),
  filename_(filename),
  line_(line)
{
  top_ = lua_gettop(L);
}

LuaStackChecker::~LuaStackChecker()
{
	int c;
	c = lua_gettop(luaState_) -  top_;
	if( c != 0 ){
		luaL_error(luaState_, "Lua stack corrupted! %d fields. File [%s] line[%d]", c , filename_, line_);
	}
}


LuaMain::LuaMain()
{
	Lst = lua_open();
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
	if( result )
		return false;

	result = lua_pcall( Lst, 0, LUA_MULTRET, 0);
    if( result )
    	return false;

    return true;
}


template<> bool LuaMain::getValue( lua_State* L, int index, bool& ret)
{
	if( lua_isboolean(L, index) ){
		ret = lua_toboolean(L, index) != 0;
		return true;
	}
	return false;
}

//FIXME: Но зачем
template<> bool LuaMain::getValue( lua_State* L, int index, float& ret)
{
	if( lua_isnumber(L, index) ){
		ret = lua_tonumber(L, index);
		return true;
	}
	return false;
}

template<> bool LuaMain::getValue( lua_State* L, int index, double& ret)
{
	if( lua_isnumber(L, index) ){
		ret = lua_tonumber(L, index);
		return true;
	}
	return false;
}

template<> bool LuaMain::getValue( lua_State* L, int index, int& ret)
{
	if( lua_isnumber(L, index) ){
		ret = static_cast<int>(lua_tonumber(L, index));
		return true;
	}
	return false;
}

template<> bool LuaMain::getValue( lua_State* L, int index, string& ret)
{
	if( lua_isstring(L, index) ){
		ret = lua_tostring(L, index);
		return true;
	}
	return false;
}