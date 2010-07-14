/*
 * Luaconfig.h
 *
 *  Created on: 04.06.2010
 */

#ifndef LUACONFIG_H_
#define LUACONFIG_H_

#include "Lua.h"

#include <sys/types.h>
#include <dirent.h>
#include <string>
using std::string;
#include <vector>

#include "debug.h"

class LuaConfig : LuaMain
{
public:
	LuaConfig( );
	~LuaConfig( );

	static LuaConfig conf;

	bool OpenConfig( string );
	bool LoadAll( string );

	string getRandom( string field, string config );

	bool getSubconfigs( string config, std::vector< string >& ret );

	template<typename T>
	bool getValue( string field, string subconfig, string config, T& ret)
	{
		const int prmsz = 3;
		const char* prm[prmsz] = { field.c_str(), subconfig.c_str(), config.c_str() };
		bool res = execFunction("get", prm, prmsz, ret);
		return res;
	}

};

#endif /* LUACONFIG_H_ */
