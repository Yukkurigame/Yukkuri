/*
 * LuaConfig.cpp
 *
 *  Created on: 04.06.2010
 */

#include "LuaConfig.h"
#include "config.h"


LuaConfig::LuaConfig( )
{

}

LuaConfig::~LuaConfig( )
{

}

bool LuaConfig::LoadAll( std::string type )
{
	const int prmsz = 1;
	bool ret;
	const char* prm[prmsz] = { type.c_str() };
	execFunction("configs:loadAll", prm, prmsz, ret);
	return ret;

}

bool LuaConfig::OpenConfig( std::string filename )
{
	const int prmsz = 1;
	bool ret;
	const char* prm[prmsz] = { filename.c_str() };
	execFunction("configs:load", prm, prmsz, ret);
	return ret;
}

//TODO: Как-то оно не так делает.
std::string LuaConfig::getRandom( std::string field, std::string config )
{
	std::string ret;
	const int prmsz = 2;
	const char* prm[prmsz] = { field.c_str(), config.c_str() };
	execFunction("configs:getOneFromSeveral", prm, prmsz, ret);
	return ret;
}

/*	Returns subconfig names list
 */
bool LuaConfig::getSubconfigsList( std::string config, list< std::string >& ret )
{
	const int prmsz = 1;
	const char* prm[prmsz] = { config.c_str() };
	bool res = execFunction( "configs:getSubconfigsList", prm, prmsz, ret );
	return res;
}

bool LuaConfig::getSubconfigsLength( std::string config, int& len )
{
	const int prmsz = 1;
	const char* prm[prmsz] = { config.c_str() };
	bool res = execFunction("configs:getSubconfigsLength", prm, prmsz, len);
	return res;
}
