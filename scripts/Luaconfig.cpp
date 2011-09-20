/*
 * Luaconfig.cpp
 *
 *  Created on: 04.06.2010
 */

#include "Luaconfig.h"
#include "config.h"


LuaConfig::LuaConfig( )
{

}

LuaConfig::~LuaConfig( )
{

}

bool LuaConfig::LoadAll( string type )
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
string LuaConfig::getRandom( string field, string config )
{
	string ret;
	const int prmsz = 2;
	const char* prm[prmsz] = { field.c_str(), config.c_str() };
	//bool res = execFunction("getOneFromSeveral", prm, prmsz, ret);
	execFunction("configs:getOneFromSeveral", prm, prmsz, ret);
	return ret;
}

/*	Returns subconfig names list
 */
bool LuaConfig::getSubconfigsList( string config, std::vector< string >& ret )
{
	const int prmsz = 1;
	const char* prm[prmsz] = { config.c_str() };
	bool res = execFunction("configs:getSubconfigsList", prm, prmsz, ret);
	return res;
}

bool LuaConfig::getSubconfigsLength( string config, int& len )
{
	const int prmsz = 1;
	const char* prm[prmsz] = { config.c_str() };
	bool res = execFunction("configs:getSubconfigsLength", prm, prmsz, len);
	return res;
}
