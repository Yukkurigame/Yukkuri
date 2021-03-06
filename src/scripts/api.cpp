/*
 * api.cpp
 *
 *  Created on: 21.09.2010
 */

#include "api.h"


#include <string>
#include <cstring>
#include <dirent.h>
#include <unistd.h>

#include "debug.h"

#ifdef _MSC_VER
	#include <direct.h>
	#define getcwd _getcwd
#endif


int scriptApi::ReadDirectory( lua_State* L )
{
	DIR *dp;
	struct dirent *ep;
	std::string dirname;
	int top;
	int count = 0;

	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Directory name not given." );

	dirname = lua_tostring( L, 1 );

	dp = opendir(dirname.c_str());
	if( dp != NULL ){
		lua_newtable( L );
		top = lua_gettop( L );
		while ( (ep = readdir( dp ) ) != NULL) {
			lua_pushinteger( L, ++count );
			lua_pushstring( L, ep->d_name );
			lua_settable( L, top );
		}
	}else{
		lua_pushnil( L );
	}
	closedir( dp );


	return 1;
}

int scriptApi::GetCWD( lua_State* L )
{
	char path[1026];

	memset( path, '\0', (unsigned)sizeof(path) );

	if( getcwd( path, (unsigned)sizeof(path) - 1 ) == NULL ){
		lua_pushnil( L );
	}else{
		lua_pushstring( L, path );
	}

	return 1;
}


int scriptApi::Debug( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Debug level expected." );
	luaL_argcheck( L, lua_isstring( L, 2 ), 2, "Debug string expected." );

	enum Debug::dbg_level level = static_cast<enum Debug::dbg_level>(lua_tointeger( L, 1 ));
	const char* str = lua_tostring( L, 2 );
	Debug::debug( level, "%s\n", str );
	lua_pop( L, lua_gettop( L ) );

	return 0;
}
