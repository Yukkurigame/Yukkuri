/*
 * Luaconfig.h
 *
 *  Created on: 04.06.2010
 */

#ifndef LUACONFIG_H_
#define LUACONFIG_H_

#include "Lua.h"

//#include <sys/types.h>
#include <string>
#include <vector>

#include "debug.h"

class LuaConfig : LuaMain
{
public:
	LuaConfig( );
	~LuaConfig( );

	bool OpenConfig( std::string );
	bool LoadAll( std::string );

	std::string getRandom( std::string field, std::string config );

	bool getSubconfigsList( std::string config, std::vector< std::string >& ret );

	bool getSubconfigsLength( std::string config, int& len );

	// Get value by subconfig-config
	template<typename T>
	bool getValue( std::string field, std::string subconfig, std::string config, T& ret)
	{
		const int prmsz = 3;
		const char* prm[prmsz] = { field.c_str(), subconfig.c_str(), config.c_str() };
		bool res = execFunction("configs:get", prm, prmsz, ret);
		return res;
	}

	// Get value by id
	template<typename T>
	bool getValue( std::string field, std::string id, T& ret)
	{
		const int prmsz = 2;
		const char* prm[prmsz] = { field.c_str(), id.c_str()};
		bool res = execFunction("configs:getById", prm, prmsz, ret);
		return res;
	}

	template<typename T>
	bool getSubconfigs( std::string config, T& ret)
	{
		const int prmsz = 1;
		const char* prm[prmsz] = { config.c_str() };
		bool res = execFunction("configs:getSubconfigs", prm, prmsz, ret);
		return res;
	}

};

#endif /* LUACONFIG_H_ */
