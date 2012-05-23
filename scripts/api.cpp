/*
 * api.cpp
 *
 *  Created on: 21.09.2010
 */

#include "api.h"
#include "Interface.h"
#include "widgets/WidgetBar.h"
#include "Bindings.h"
#include "LuaConfig.h"
#include "unitmanager.h"
#include "LuaThread.h"
#include "Camera.h"
#include "map.h"
#include <string>
#include <cstring>
#include <dirent.h>

#include "debug.h"
using namespace Debug;


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

	memset( path, '\0', sizeof(path) );

	if( getcwd( path, sizeof(path) - 1 ) == NULL ){
		lua_pushnil( L );
	}else{
		lua_pushstring( L, path );
	}

	return 1;
}


int scriptApi::Debug( lua_State* L )
{
	enum dbg_level level;
	std::string str;
	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Debug level expected." );
	luaL_argcheck( L, lua_isstring( L, 2 ), 2, "Debug string expected." );

	level = static_cast<enum dbg_level>(lua_tointeger( L, 1 ));
	str = lua_tostring( L, 2 );
	debug( level, str + "\n" );
	lua_pop( L, lua_gettop( L ) );
	return 0;
}

int scriptApi::SetBindings( lua_State* L )
{
	std::string bname;
	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Bindings id not given." );

	bname = lua_tostring( L, 1 );

	Bindings::bnd.LoadKeys( bname );

	return 0;
}


int scriptApi::DeleteUnit( lua_State* L )
{
	unsigned int id;
	Unit* u;

	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Unit id not given." );

	id = static_cast<int>(lua_tointeger( L, 1 ));
	u = UnitManager::GetUnit( id );
	lua_pop( L, lua_gettop( L ) );
	if( !id || !u ){
		lua_pushboolean( L, false );
		return 1;
	}
	UnitManager::DeleteUnit( u );
	lua_pushboolean( L, true );

	return 1;
}


int scriptApi::LoadMapRegion(lua_State *L)
{
	std::string name;
	extern Map map;
	luaL_argcheck( L, lua_isstring( L, 1 ), 1, "Parameter not given." );
	name = lua_tostring( L, 1 );
	map.Load( name );
	return 0;
}


