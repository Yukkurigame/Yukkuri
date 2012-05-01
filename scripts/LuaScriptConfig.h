/*
 * LuaScriptConfig.h
 *
 *  Created on: 19.04.2012
 *
 */

#ifndef LUASCRIPTCONFIG_H_
#define LUASCRIPTCONFIG_H_

#include "LuaScript.h"
#include "LuaConfig.h"

class LuaScriptConfig: public LuaScript, public LuaConfig
{
public:

	// Get value by subconfig-config
	template<typename T1, typename T2>
	bool getValue( std::string field, std::string subconfig, std::string config, T1& ret1, T2& ret2)
	{
		const int prmsz = 3;
		const char* prm[prmsz] = { field.c_str(), subconfig.c_str(), config.c_str() };
		bool res = execFunction("configs:get", prm, prmsz, ret1, ret2);
		return res;
	}

	//TODO: exec without returned values
	template<typename T1, typename T2>
	bool execFunction( std::string function, const char* params[], const int sz, T1& ret1, T2& ret2)
	{
		LuaStackChecker sc(Lst, __FILE__, __LINE__);

		int szadd =  LuaScript::execFunction( function );

		for( int i=0; i<sz; ++i ){
			lua_pushstring( Lst, params[i] );
		}
		if( lua_pcall( Lst, sz + szadd, 1, 0 ) ){
			std::string err;
			LuaScript::getValue( Lst, -1, err );
			debug( Debug::SCRIPT, "Lua function '" + function + "' execute error: " + err + "\n" );
			lua_pop( Lst, 1 + szadd );
			return false;
		}
		bool res = getValue( Lst, -1, ret1, ret2 );
		//TODO: szadd должно работать при :, но это неправильный костыль, больше негде проверить.
		lua_pop( Lst, 1 + szadd );
		return res;
	}

	template<typename T1Key, typename T1Value, typename T2Key>
	bool getValue( lua_State* L, int index, std::map<T1Key, T1Value>& ret,  std::map<T2Key, LuaRegRef>& funcret)
	{
		if(!lua_istable(L, index))
			return false;

		LuaStackChecker sc(L, __FILE__, __LINE__);

		lua_pushvalue(L, index); // stack: map
		lua_pushnil(L); // stack: map nil
		while(lua_next(L, -2)){
			// stack: map key value
			if( lua_isfunction(L, -1) ){
				T2Key key;
				LuaScript::getValue( L, -2, key );
				LuaRegRef value;
				RegProc( L, &value, -1); // stack: map key
				funcret[key] = value;
			}else{
				T1Key key;
				LuaScript::getValue( L, -2, key );
				T1Value value;
				LuaScript::getValue( L, -1, value );
				ret[key] = value;
				lua_pop(L, 1); // stack: map key
			}
		}

		// stack: map
		lua_pop(L, 1); // stack:

		return true;
	}

};


#endif /* LUASCRIPTCONFIG_H_ */
