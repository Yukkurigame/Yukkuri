/*
 * LuaPusher.cpp
 *
 *  Created on: 18.01.2013
 *
 * Extended types for 3rdparty/LuaPusher.h
 *
 */

#include "3rdparty/LuaPusher.h"
#include "scripts/Lua.h"
#include "basic_types.h"
#include "graphics/utils/gl_shader.h"

#include "safestring.h"



// s2f
template <>
int pushToLua(lua_State* L, s2f const& val)
{
	const float* p[2] = { &val.x, &val.y };
	const char* pc[2] = { "x", "y" };
	lua_newtable(L);
	for( int i=0; i<2; ++i ){
		lua_pushstring( L, pc[i] );
		lua_pushnumber( L, *p[i] );
		lua_settable(L, -3);
	}
	return 1;
}

template<>
s2f const getFromLua(lua_State* L, int idx)
{
	s2f ret;
	float* r[2] = { &ret.x, &ret.y };
	lua_pop(L, 1); // stack: vector
	for( int i = 1; i < 3; ++i ){
		lua_pushnumber(L, i);
		lua_gettable(L, -2);
		getFromLua( L, -1, *r[i] );
		lua_pop(L, 1);
	}
	return ret;
}

template<>
void getFromLua(lua_State* L, int idx, s2f& val)
{
	float* r[2] = { &val.x, &val.y };
	lua_pop(L, 1); // stack: vector
	for( int i = 1; i < 3; ++i ){
		lua_pushnumber(L, i);
		lua_gettable(L, -2);
		getFromLua( L, -1, *r[i] );
		lua_pop(L, 1);
	}
}

template<>
bool CHINP_TESTER<s2f>(lua_State* L, int idx)
{
	if( lua_istable( L, idx ) && luaL_getn( L, idx ) == 2 ){
		for( int i = 1; i < 3; ++i ){
			lua_pushnumber( L, i );
			lua_gettable( L, -2 );
			if( lua_isnumber( L, -1 ) == 0 ){
				lua_pop(L, 1);
				return false;
			}
			lua_pop(L, 1);
		}
		return true;
	}
	return false;
}



// ShaderConfigData
template <>
int pushToLua( lua_State* L, ShaderConfigData* const& val )
{
	// NOT IMPLEMENTED. Why?
	return 0;
}


template<>
ShaderConfigData const getFromLua( lua_State* L, int idx )
{
	ShaderConfigData ret;
	// NOT IMPLEMENTED. Why?
	return ret;
}

template<>
void getFromLua( lua_State* L, int idx, ShaderConfigData& val )
{
	// Get filenames
	char** filenames[2] = { &val.vertex_name, &val.fragment_name };
	lua_pushstring( L, "files" );	// st: table files
	lua_gettable( L, -2 );			// st: table table{file1, file2}
	for( int i = 0; i < 2; ++i ){
		lua_pushnumber( L, i );
		lua_gettable( L, -2 );
		getFromLua( L, -1, *filenames[i] );
		lua_pop( L, 1 );
	}
	lua_pop(L, 1);					// st: table

	// Get output
	lua_pushstring( L, "output" );	// st: table output
	lua_gettable( L, -2 );			// st: table table
	val.output_count = luaL_getn( L, -1 );
	if( val.output_count ){
		val.output = (char**)malloc( (unsigned)sizeof(char*) * val.output_count );
		for( unsigned int i = 0; i < val.output_count; ++i ){
			lua_pushnumber( L, i );
			lua_gettable( L, -2 );
			val.output[i] = NULL;
			getFromLua( L, -1, val.output[i] );
			lua_pop( L, 1 );
		}
	}
	lua_pop( L, 1 );

	// Get attributes
	lua_pushstring( L, "attributes" );	// st: table attributes
	lua_gettable( L, -2 );				// st: table table
	val.attributes_count = luaL_getn( L, -1 );
	if( val.attributes_count ){
		val.attributes = new ShaderConfigAttributes[val.attributes_count];
		for( unsigned int i = 0; i < val.attributes_count; ++i ){
			lua_pushnumber( L, i );
			lua_gettable( L, -2 );		// st: table table table{int, string}

			lua_pushnumber( L, 0 );
			lua_gettable( L, -2 );		// st: table table table int
			getFromLua( L, -1, val.attributes[i].index );
			lua_pop( L, 1 );

			lua_pushnumber( L, 1 );
			lua_gettable( L, -2 );		// st: table table table int
			getFromLua( L, -1, val.attributes[i].name );
			lua_pop( L, 1 );

			lua_pop( L, 1 );			// st: table table
		}
	}
	lua_pop( L, 1 );					// st: table
}

template<>
bool CHINP_TESTER<ShaderConfigData>(lua_State* L, int idx)
{
	if( lua_istable( L, idx ) ){
		lua_pushstring( L, "type" );
		lua_gettable( L, -2 );
		if( lua_isstring( L, 1 ) &&
			!strcmp( "shader", lua_tostring( L, 1 ) ) )
			return true;
	}
	return false;
}
