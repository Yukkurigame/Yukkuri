/*
 * Lua.h
 *
 *  Created on: 13.07.2010
 */

#ifndef LUA_H_
#define LUA_H_

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

#include <map>
#include <vector>
#include <string>
using std::string;

#include "debug.h"

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

		int szadd = execFunction( function );

		for( int i=0; i<sz; ++i ){
			lua_pushstring( Lst, params[i] );
		}
		if( lua_pcall( Lst, sz + szadd, 1, 0 ) ){
			string err;
			getValue( Lst, -1, err );
			debug( 4, "Lua function '" + function + "' execute error: " + err + "\n" );
			lua_pop( Lst, 1 + szadd );
			return false;
		}
		bool res = getValue( Lst, -1, ret );
		//TODO: szadd должно работать при :, но это неправильный костыль, больше негде проверить.
		lua_pop( Lst, 1 + szadd );
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
		for(int i = 1; count >= i; ++i)	{
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
	static lua_State* Lst;

private:
	static int count;
	int execFunction( string );

};



#endif /* LUA_H_ */
