/*
 * Luaconfig.h
 *
 *  Created on: 04.06.2010
 */

#ifndef LUACONFIG_H_
#define LUACONFIG_H_

extern "C" {
# include "lua.h"
# include "lauxlib.h"
# include "lualib.h"
}

#include <string>
using std::string;

class LuaConfig
{
public:
	LuaConfig( );
	~LuaConfig( );

	bool OpenConfig( string );

	double getNumber( string );
	string getString( string );

private:
	lua_State *Lconf;

};


#endif /* LUACONFIG_H_ */
