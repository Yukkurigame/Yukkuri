/*
 * ShaderConfigData.cpp
 *
 *  Created on: 30.01.2013
 */

#include "3rdparty/LuaPusher.h"
#include "scripts/Lua.h"
#include "graphics/utils/gl_shader.h"


// ShaderConfigData
template <>
int pushToLua( lua_State* L, ShaderConfigData* const& val )
{
	// NOT IMPLEMENTED. Why?
	return 0;
}

template<>
void getFromLua( lua_State* L, int idx, ShaderConfigData& val )
{
	// Get id
	LUA_GET_VALUE( "name", val.name )

	// Get filenames
	char** filenames[2] = { &val.vertex_name, &val.fragment_name };
	lua_pushstring( L, "files" );	// st: table files
	lua_gettable( L, -2 );			// st: table table{file1, file2}
	for( int i = 0; i < 2; ++i ){
		LUA_GET_TABLE_VALUE( i + 1, *filenames[i] )
	}
	lua_pop(L, 1);					// st: table

	// Get output
	lua_pushstring( L, "output" );	// st: table output
	lua_gettable( L, -2 );			// st: table table
	val.output_count = luaL_getn( L, -1 );
	if( val.output_count ){
		val.output = (char**)malloc( (unsigned)sizeof(char*) * val.output_count );
		for( unsigned int i = 0; i < val.output_count; ++i ){
			val.output[i] = NULL;
			LUA_GET_TABLE_VALUE( i + 1, val.output[i] )
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
			lua_pushnumber( L, i + 1 );
			lua_gettable( L, -2 );		// st: table table table{int, string}

			LUA_GET_TABLE_VALUE( 1, val.attributes[i].index )
			LUA_GET_TABLE_VALUE( 2, val.attributes[i].name )

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
		if( lua_isstring( L, -1 ) && !strcmp( "shader", lua_tostring( L, -1 ) ) ){
			lua_pop( L, 1 );
			return true;
		}
		lua_pop( L, 1 );
	}
	return false;
}


