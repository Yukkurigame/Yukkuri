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
	for( int i = 0; i < 2; ++i ){
		LUA_GET_TABLE_VALUE( i + 1, *r[i] )
	}
	return ret;
}

template<>
void getFromLua(lua_State* L, int idx, s2f& val)
{
	float* r[2] = { &val.x, &val.y };
	for( int i = 0; i < 2; ++i ){
		LUA_GET_TABLE_VALUE( i + 1, *r[i] )
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


// rect2f
template <>
int pushToLua(lua_State* L, rect2f const& val)
{
	const float* p[4] = { &val.x, &val.y, &val.width, &val.height };
	const char* pc[4] = { "x", "y", "width", "height" };
	lua_newtable(L);
	for( int i=0; i<4; ++i ){
		lua_pushstring( L, pc[i] );
		lua_pushnumber( L, *p[i] );
		lua_settable(L, -3);
	}
	return 1;
}

template<>
rect2f const getFromLua(lua_State* L, int idx)
{
	rect2f ret;
	float* r[4] = { &ret.x, &ret.y, &ret.width, &ret.height };
	const char* pc[4] = { "x", "y", "width", "height" };
	for( int i = 0; i < 4; ++i ){
		LUA_GET_VALUE( pc[i], *r[i] )
	}
	return ret;
}

template<>
void getFromLua( lua_State* L, int idx, rect2f& val )
{
	float* r[4] = { &val.x, &val.y, &val.width, &val.height };
	const char* pc[4] = { "x", "y", "width", "height" };
	for( int i = 0; i < 4; ++i ){
		LUA_GET_VALUE( pc[i], *r[i] )
	}
}

template<>
bool CHINP_TESTER<rect2f>(lua_State* L, int idx)
{
	const char* pc[4] = { "x", "y", "width", "height" };
	if( lua_istable( L, idx ) ){
		for( int i = 0; i < 4; ++i ){
			lua_pushstring( L, pc[i] );
			lua_gettable( L, -2 );
			if( !lua_isnumber( L, -1 ) &&
				!lua_isnoneornil( L, -1 ) ){
					lua_pop(L, 1);
					return false;
			}
			lua_pop(L, 1);
		}
		return true;
	}
	return false;
}


// s4ub
template <>
int pushToLua( lua_State* L, s4ub const& val )
{
	const GLubyte* p[4] = { &val.r, &val.g, &val.b, &val.a };
	const char* pc[4] = { "r", "g", "b", "a" };
	lua_newtable(L);
	for( int i=0; i<4; ++i ){
		lua_pushstring( L, pc[i] );
		lua_pushnumber( L, *p[i] );
		lua_settable(L, -3);
	}
	return 1;
}

template<>
s4ub const getFromLua( lua_State* L, int idx )
{
	s4ub ret;
	GLubyte* r[4] = { &ret.r, &ret.g, &ret.b, &ret.a };
	for( int i = 0; i < 4; ++i ){
		LUA_GET_TABLE_VALUE( i + 1, *r[i] )
	}
	return ret;
}

template<>
void getFromLua( lua_State* L, int idx, s4ub& val )
{
	GLubyte* r[4] = { &val.r, &val.g, &val.b, &val.a };
	for( int i = 0; i < 4; ++i ){
		LUA_GET_TABLE_VALUE_DEFAULT( i + 1, *r[i], (i > 2 ? 255 : 0) )
	}
}

template<>
bool CHINP_TESTER<s4ub>(lua_State* L, int idx)
{
	if( lua_istable( L, idx ) ){
		for( int i = 1; i < 4; ++i ){
			lua_pushnumber( L, i );
			lua_gettable( L, -2 );
			if( !lua_isnumber( L, -1 ) &&
				!lua_isnone( L, -1 ) ){
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

	// Only if samplers was allocated
	if( val.samplers != NULL ){
		lua_pushstring( L, "samplers" );
		lua_gettable( L, -2 );
		val.samplers->count = luaL_getn( L, -1 );
		if( val.samplers->count ){
			val.samplers->data = (char**)malloc( (unsigned)sizeof(char*) * val.samplers->count );
			for( unsigned int i = 0; i < val.samplers->count; ++i ){
				val.samplers->data[i] = NULL;
				LUA_GET_TABLE_VALUE( i + 1, val.samplers->data[i] )
			}
		}
		lua_pop( L, 1 );
	}

	lua_pushstring( L, "uniforms" );
	lua_gettable( L, -2 );
	val.uniforms_count = luaL_getn( L, -1 );
	if( val.uniforms_count ){
		val.uniforms = new ShaderConfigAttributes[val.uniforms_count];
		for( unsigned int i = 0; i < val.uniforms_count; ++i ){
			lua_pushnumber( L, i + 1 );
			lua_gettable( L, -2 );		// st: table table table{string, int}

			LUA_GET_TABLE_VALUE( 1, val.uniforms[i].name )
			LUA_GET_TABLE_VALUE( 2, val.uniforms[i].index )

			lua_pop( L, 1 );			// st: table table
		}
	}
	lua_pop( L, 1 );

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

