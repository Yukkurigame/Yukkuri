/*
 * LuaConfig.h
 *
 *  Created on: 04.06.2010
 */

#ifndef LUACONFIG_H_
#define LUACONFIG_H_

#include "Lua.h"
#include <string>

class LuaConfig : public LuaMain
{
public:
	LuaConfig( );
	~LuaConfig( );

	bool OpenConfig( std::string );
	bool LoadAll( std::string );

	std::string getRandom( std::string field, std::string config );

	bool getSubconfigsList( std::string config, list< std::string >& ret );

	bool getSubconfigsLength( std::string config, int& len );

	bool pushSubconfig( const char* subconfig, const char* config ){
		const int prmsz = 2;
		const char* prm[prmsz] = { subconfig, config };
		return execFunction("configs:getSubconfig", prm, prmsz) > 0;
	}

	// Get value by subconfig-config
	template<typename T>
	bool getValue( const char* field, const char* subconfig, const char* config, T& ret)
	{
		const int prmsz = 3;
		const char* prm[prmsz] = { field, subconfig, config };
		return execFunction("configs:get", prm, prmsz, ret);
	}

	// Get value by id
	template<typename T>
	bool getValue( const char* field, const char* id, T& ret)
	{
		const int prmsz = 2;
		const char* prm[prmsz] = { field, id };
		return execFunction("configs:getById", prm, prmsz, ret);
	}

	template<typename T>
	bool getSubconfigs( const char* config, T& ret)
	{
		const int prmsz = 1;
		const char* prm[prmsz] = { config };
		return execFunction("configs:getSubconfigs", prm, prmsz, ret);
	}

};

#endif /* LUACONFIG_H_ */
