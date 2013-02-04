/*
 * Lua.cpp
 *
 *  Created on: 13.07.2010
 */

#include "Lua.h"
#include "types.h"

#include <cstring>
#include <cstdlib>

#include "debug.h"
using namespace Debug;


int LuaMain::count = 0;
lua_State* LuaMain::Lst = NULL;


LuaStackChecker::LuaStackChecker( lua_State* L, const char* filename/* = ""*/, int line/* = 0*/ )
: luaState_(L),
filename_(filename),
line_(line)
{
	top_ = lua_gettop(L);
}

LuaStackChecker::~LuaStackChecker( )
{
	int c;
	c = lua_gettop( luaState_ ) -  top_;
	if( c != 0 ){
		luaL_error( luaState_, "Lua stack corrupted! %d fields. File [%s] line[%d]\n", c , filename_, line_ );
	}
}


LuaMain::LuaMain()
{
	count++;
}

LuaMain::~LuaMain()
{
	count--;

	if( count == 0 ){
		lua_close(Lst);
		Lst = NULL;
		Debug::debug( Debug::SCRIPT, "Lua engine closed.\n" );
	}
}

bool LuaMain::init( )
{
	if( Lst == NULL ){
		Lst = luaL_newstate();
		luaL_openlibs( Lst );
		RegisterApi( Lst );
		return true;
	}
	return false;
}


bool LuaMain::OpenFile( const char* name )
{
	int result;

	result = luaL_loadfile( Lst, name );
	if( result ){
		debug( SCRIPT, "Open %s failed: %s.\n", name, lua_tostring( Lst, -1 ) );
		lua_pop( Lst, 1 );
		return false;
	}

	result = lua_pcall( Lst, 0, 0, 0);
	if( result ){
		debug( SCRIPT, "Execute of %s failed: %s.\n", name, lua_tostring( Lst, -1 ) );
		lua_pop( Lst, 1 );
		return false;
	}

	return true;
}

template<> void initValue( bool& ret ) { ret = false; }
template<> void initValue( float& ret ) { ret = 0.0f; }
template<> void initValue( double& ret ) { ret = 0.0; }
template<> void initValue( unsigned int& ret ) { ret = 0; }
template<> void initValue( int& ret ) { ret = 0; }
template<> void initValue( std::string& ret ) { ret = ""; }
template<> void initValue( char*& ret ) { ret = '\0'; }



int LuaMain::execFunction( const char* function )
{
	//int cnum;
	int szadd = 0;
	UINT fnsize = sizeof(char) * strlen(function);
	char* funcname = (char*)malloc( fnsize );
	char* subfuncname = (char*)malloc( fnsize );
	char delimetr = '\0';

	memset( funcname, '\0', fnsize );
	memset( subfuncname, '\0', fnsize );

	//FIXME: needs refactoring
	const char* sptr = strchr( function, '.' );
	if( !sptr )
		sptr = strchr( function, ':' );

	if( sptr ){
		delimetr = sptr[0];
		strncpy( funcname, function, (UINT)(sptr - function) );
		sptr++;
		strncpy( subfuncname, sptr, strlen(sptr) );
	}else{
		strncpy( funcname, function, fnsize );
	}

	lua_getfield( Lst, LUA_GLOBALSINDEX, funcname );
	if( delimetr ){
		lua_getfield( Lst, -1, subfuncname );
		if( delimetr == ':' ){
			lua_pushvalue( Lst, -2 );
			szadd++;
		}
	}

	free( funcname );
	free( subfuncname );

	return szadd;

}
