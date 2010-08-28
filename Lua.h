/*
 * Lua.h
 *
 *  Created on: 13.07.2010
 */

#ifndef LUA_H_
#define LUA_H_

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

#include <map>
#include <vector>
#include <string>
using std::string;

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


class LuaMain
{
public:
	LuaMain( );
	~LuaMain( );

	bool OpenFile( string name );

	//Какого хрена шаблоны перестают работать, если их вынести?
	//FIXME: move from header

	//TODO: exec without returned values
	template<typename T>
	bool execFunction( string function, const char* params[], const int sz, T& ret)
	{
		LuaStackChecker sc(Lst, __FILE__, __LINE__);
		lua_getfield( Lst, LUA_GLOBALSINDEX, function.c_str() );
		for( int i=0; i<sz; ++i ){
			lua_pushstring( Lst, params[i] );
		}
		lua_call( Lst, sz, 1 );
		bool res = getValue( Lst, -1, ret );
		lua_pop( Lst, 1 );
		return res;
	}

	template<typename T>
	bool getValue( lua_State* L, int index, T& ret);


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

protected:
	lua_State *Lst;
};



#endif /* LUA_H_ */
