/*
 * Luaconfig.h
 *
 *  Created on: 04.06.2010
 */

#ifndef LUACONFIG_H_
#define LUACONFIG_H_

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

#include <sys/types.h>
#include <dirent.h>
#include <string>
using std::string;
#include <vector>
#include <map>

#include <iostream>
using std::cout;
using std::endl;


class LuaStackChecker
{
public:
  LuaStackChecker(lua_State* L, const char* filename = "", int line = 0);
  ~LuaStackChecker();

private:
  lua_State* luaState_;
  const char* filename_;
  int line_;
  int top_;
};


class LuaConfig
{
public:
	LuaConfig( );
	~LuaConfig( );

	static LuaConfig conf;

	bool OpenConfig( string );
	bool LoadAll( string );

	bool setDefault( string );
	bool execFunction( string, string );

	string getRandom( string field );

	template<typename T>
	bool getValue( lua_State* L, int index, T& ret);

	//Какого хрена шаблоны перестают работать, если их вынести?
	//FIXME: move from header

	template<typename T>
	bool getValue( lua_State* L, int index, std::vector<T>& ret )
	{
	  // stack:
	  if(!lua_istable(L, index))
	    return false;

	  LuaStackChecker sc(L, __FILE__, __LINE__);

	  lua_pushvalue(L, index); // stack: vector

	  const int count = luaL_getn(L, -1);
	  for(int i = 1; count >= i; ++i)
	  {
	    lua_pushnumber(L, i);
	    lua_gettable(L, -2);
	    T value;
	    getValue(L, -1, value);
	    ret.push_back(value);
	    lua_pop(L, 1); // stack: vector
	  }
	  lua_pop(L, 1); // stack:

	  return true;
	}


	template<typename TKey, typename TValue>
	bool getValue( lua_State* L, int index, std::map<TKey, TValue>& ret )
	{
		if(!lua_istable(L, index))
			return false;

		LuaStackChecker sc(L, __FILE__, __LINE__);

		lua_pushvalue(L, index); // stack: map
		lua_pushnil(L); // stack: map nil
		//cout << 's' << lua_gettop(L) << 't'  << lua_type(L, -2) << endl;
		while(lua_next(L, -2)){ // stack: map key value
			TKey key;
			getValue(L, -2, key);
			TValue value;
			getValue(L, -1, value);
			ret[key] = value;
			lua_pop(L, 1); // stack: map key
		}

		// stack: map
		lua_pop(L, 1); // stack:

		return true;
	}

	template<typename T>
	bool getValue( string field, T& ret)
	{
		LuaStackChecker sc( Lconf, __FILE__, __LINE__ );
		lua_getfield( Lconf, LUA_GLOBALSINDEX, "get" );
		lua_pushstring( Lconf, field.c_str() );
		lua_call( Lconf, 1, 1 );
		bool res = getValue( Lconf, -1, ret );
		lua_pop( Lconf, 1 );
		return res;
	}

private:
	lua_State *Lconf;

};

#endif /* LUACONFIG_H_ */
