/*
 * Lua.h
 *
 *  Created on: 13.07.2010
 */

#ifndef LUA_H_
#define LUA_H_


#define LUALIBFIELD( f ) { #f, f },
#define LUALIBEND {NULL, NULL}

extern "C" {
	#include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}

#include <map>
#include <vector>
#include <string>
#include <utility>

#include "3rdparty/LuaPusher.h"

#include "debug.h"

struct LuaRet {
	int c;
	LuaRet() : c() {};
	LuaRet( int n ) : c( n ) { };
};


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

	bool init( );

	static lua_State* getState() { return Lst; }

	void RegisterApi( lua_State* L );
	void RegisterLib( std::string name, const luaL_Reg* functions );

	bool OpenFile( std::string name );

	template <typename T>
	inline int push( T const & val ){
		return pushToLua( Lst, val );
	}

	//Какого хрена шаблоны перестают работать, если их вынести?
	//FIXME: move from header

	//TODO: exec without returned values
	template<typename T>
	bool execFunction( std::string function, const char* params[], const int sz, T& ret)
	{
		LuaStackChecker sc(Lst, __FILE__, __LINE__);

		int szadd = execFunction( function );

		for( int i=0; i<sz; ++i ){
			lua_pushstring( Lst, params[i] );
		}
		if( lua_pcall( Lst, sz + szadd, 1, 0 ) ){
			std::string err;
			getValue( Lst, -1, err );
			debug( Debug::SCRIPT, "Lua function '" + function + "' execute error: " + err + "\n" );
			lua_pop( Lst, 1 + szadd );
			return false;
		}
		bool res = getValue( Lst, -1, ret );
		//TODO: szadd должно работать при :, но это неправильный костыль, больше негде проверить.
		lua_pop( Lst, 1 + szadd );
		return res;
	}

	inline const int top(){
		return lua_gettop( Lst );
	}

	inline int getType( int index ){
		return lua_type( Lst, index );
	}

	inline bool getBool( int index ){
		if( lua_isboolean( Lst, index ) )
			return lua_toboolean( Lst, index ) != 0;
		return false;
	}
	inline double getNumber( int index ){
		if( lua_isnumber( Lst, index ) )
			return lua_tonumber( Lst, index );
		return 0.0;
	}
	inline const char* getChar( int index ){
		if( lua_isstring( Lst, index) )
			return lua_tostring( Lst, index );
		return NULL;
	}
	inline std::string getString( int index ){
		if( lua_isstring( Lst, index) )
			return (std::string)lua_tostring( Lst, index );
		return "";
	}

	template<typename T>
	bool getValue( lua_State* L, int index, T& ret);

	template<typename T1, typename T2>
	bool getValue( lua_State* L, int index, std::pair<T1, T2>& ret )
	{
		// stack:
		if(!lua_istable(L, index))
			return false;

		LuaStackChecker sc(L, __FILE__, __LINE__);

		lua_pushvalue(L, index); // stack: pair
		lua_pushnumber(L, 1); // stack: pair first
		lua_gettable(L, -2); // stack: pair value
		getValue(L, -1, ret.first);
		lua_pop(L, 1); // stack: pair
		lua_pushnumber(L, 2);
		lua_gettable(L, -2);
		getValue(L, -1, ret.second); // stack: pair value2
		lua_pop(L, 2); // stack:

		return true;
	}


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

	template<typename T>
	void getValueByName( lua_State* L, const char* name, T& ret, bool check=false )
	{
		lua_getfield( L, -1, name );			// stack: table, [name]
		if( !check || !lua_isnone( L, -1 ) )
			getValue( L, -1, ret );
		lua_pop(L, 1);                      // stack: table
	}


protected:
	int execFunction( std::string );
	static lua_State* Lst;

private:
	static int count;

};



#endif /* LUA_H_ */
