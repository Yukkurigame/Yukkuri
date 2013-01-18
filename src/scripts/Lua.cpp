/*
 * Lua.cpp
 *
 *  Created on: 13.07.2010
 */
#include "Lua.h"
#include "graphics/GraphicsTypes.h"
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


bool LuaMain::OpenFile( std::string name )
{
	int result;

	result = luaL_loadfile( Lst, name.c_str() );
	if( result ){
		std::string e = lua_tostring( Lst, -1 );
		debug( SCRIPT, "Open " + name + " failed " + e + "\n" );
		return false;
	}

	result = lua_pcall( Lst, 0, 0, 0);
	if( result ){
		std::string e = lua_tostring( Lst, -1 );
		debug( SCRIPT, "Execute " + name + " failed: " + e + "\n" );
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
template<> void initValue( rect2f& ret ) { ret = rect2f(); }
template<> void initValue( s4ub& ret ) { ret = s4ub(); }


/*
template<> bool LuaMain::getValue( lua_State* L, int index, bool& ret)
{
	ret = false;
	if( lua_isboolean(L, index) ){
		ret = lua_toboolean(L, index) != 0;
		return true;
	}
	return false;
}

//FIXME: ���� ����������
template<> bool LuaMain::getValue( lua_State* L, int index, float& ret)
{
	ret = 0.0;
	if( lua_isnumber(L, index) ){
		ret = static_cast<float>(lua_tonumber(L, index));
		return true;
	}
	return false;
}

template<> bool LuaMain::getValue( lua_State* L, int index, double& ret)
{
	ret = 0.0;
	if( lua_isnumber(L, index) ){
		ret = lua_tonumber(L, index);
		return true;
	}
	return false;
}

template<> bool LuaMain::getValue( lua_State* L, int index, unsigned int& ret)
{
	ret = 0;
	if( lua_isnumber(L, index) ){
		ret = static_cast<int>(lua_tonumber(L, index));
		return true;
	}
	return false;
}

template<> bool LuaMain::getValue( lua_State* L, int index, int& ret)
{
	ret = 0;
	if( lua_isnumber(L, index) ){
		ret = static_cast<int>(lua_tonumber(L, index));
		return true;
	}
	return false;
}

template<> bool LuaMain::getValue( lua_State* L, int index, std::string& ret)
{
	ret = "";
	if( lua_isstring(L, index) ){
		ret = lua_tostring(L, index);
		return true;
	}
	return false;
}

template<> bool LuaMain::getValue( lua_State* L, int index, char*& ret)
{
	if( lua_isstring(L, index) ){
		const char* lstring = lua_tostring(L, index);
		ret = strdup( lstring );
		return true;
	}
	ret = '\0';
	return false;
}

// Color
template<> bool LuaMain::getValue( lua_State* L, int index, s4ub& ret)
{
	if( lua_istable(L, index) ){
		unsigned int val[4];
		const int count = luaL_getn(L, -1);
		for( int i = 1; i < 5; ++i ){
			// TODO: default behavior is strange
			if( i > count ){
				val[i-1] = i < 4 ? 0 : 255;
			}else{
				lua_pushnumber(L, i);
				lua_gettable(L, -2);
				getValue(L, -1, val[i-1]);
				lua_pop(L, 1); // stack: vector
			}
		}
		ret.set( val[0], val[1], val[2], val[3] );
		return true;
	}
	return false;
}

// rect2f
template<> bool LuaMain::getValue( lua_State* L, int index, rect2f& ret)
{
	if( lua_istable(L, index) ){
		const char* val[4] = { "x", "y", "width", "height" };
		float* r[4] = { &ret.x, &ret.y, &ret.width, &ret.height };
		for( int i = 0; i < 4; ++i ){
			lua_getfield( L, -1, val[i] );	// stack: table value
			getValue( L, -1, *r[i] );
			lua_pop(L, 1); 					// stack: table
		}
		return true;
	}
	return false;
}
*/

int LuaMain::execFunction( std::string function )
{
	int cnum;
	int szadd = 0;
	char* funcname = (char*)malloc( (unsigned)(sizeof(char) * function.length()) );
	char* subfuncname = (char*)malloc( (unsigned)(sizeof(char) * function.length()) );
	char delimetr = '\0';

	//FIXME: needs refactoring
	cnum = function.find('.');
	if( cnum >= 0 ){
		delimetr = '.';
	}else{
		cnum = function.find(':');
		if( cnum >= 0 ){
			delimetr = ':';
		}
	}
	if( delimetr != '\0' ){
		// FIXME: strdup?
		strcpy(funcname, function.substr( 0, cnum ).c_str( ));
		strcpy(subfuncname, function.substr( cnum +1, function.length( ) ).c_str( ));
	}else{
		strcpy(funcname, function.c_str( ));
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
