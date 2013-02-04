/*
 * LuaConfig.cpp
 *
 *  Created on: 04.06.2010
 */

#include "LuaConfig.h"


LuaConfig::LuaConfig( )
{

}

LuaConfig::~LuaConfig( )
{

}

bool LuaConfig::LoadAll( const char* type )
{
	const int prmsz = 1;
	bool ret;
	const char* prm[prmsz] = { type };
	execFunction("configs:loadAll", prm, prmsz, ret);
	return ret;

}

bool LuaConfig::OpenConfig( const char* filename )
{
	const int prmsz = 1;
	bool ret;
	const char* prm[prmsz] = { filename };
	execFunction("configs:load", prm, prmsz, ret);
	return ret;
}

//TODO: Как-то оно не так делает.
char* LuaConfig::getRandom( const char* field, const char* config )
{
	char* ret = NULL;
	const int prmsz = 2;
	const char* prm[prmsz] = { field, config };
	execFunction("configs:getOneFromSeveral", prm, prmsz, ret);
	return ret;
}

/*	Returns subconfig names list
 */
bool LuaConfig::getSubconfigsList( const char* config, list< char* >& ret )
{
	const int prmsz = 1;
	const char* prm[prmsz] = { config };
	bool res = execFunction( "configs:getSubconfigsList", prm, prmsz, ret );
	return res;
}

bool LuaConfig::getSubconfigsLength( const char* config, int& len )
{
	const int prmsz = 1;
	const char* prm[prmsz] = { config };
	bool res = execFunction("configs:getSubconfigsLength", prm, prmsz, len);
	return res;
}
