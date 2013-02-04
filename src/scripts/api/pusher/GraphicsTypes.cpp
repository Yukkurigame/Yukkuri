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
#include "graphics/GraphicsTypes.h"


// s2f
template <>
int pushToLua(lua_State* L, s2f const& val)
{
	const float* p[2] = { &val.x, &val.y };
	const char* pc[2] = { "x", "y" };
	lua_newtable(L);
	for( int i=0; i<2; ++i ){
		LUA_PUSH_VALUE( number, pc[i], *p[i] )
	}
	return 1;
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


// s3f
template <>
int pushToLua(lua_State* L, s3f const& val)
{
	const float* p[3] = { &val.x, &val.y, &val.z };
	const char* pc[3] = { "x", "y", "z" };
	lua_newtable(L);
	for( int i = 0; i < 3; ++i ){
		LUA_PUSH_VALUE( number, pc[i], *p[i] )
	}
	return 1;
}

template<>
void getFromLua(lua_State* L, int idx, s3f& val)
{
	float* r[3] = { &val.x, &val.y, &val.z };
	for( int i = 0; i < 3; ++i ){
		LUA_GET_TABLE_VALUE( i + 1, *r[i] )
	}
}

template<>
bool CHINP_TESTER<s3f>( lua_State* L, int idx )
{
	if( lua_istable( L, idx ) && luaL_getn( L, idx ) == 3 ){
		for( int i = 1; i < 4; ++i ){
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
template<> void initValue( rect2f& ret ) { ret = rect2f(); }

template <>
int pushToLua(lua_State* L, rect2f const& val)
{
	const float* p[4] = { &val.x, &val.y, &val.width, &val.height };
	const char* pc[4] = { "x", "y", "width", "height" };
	lua_newtable(L);
	for( int i = 0; i < 4; ++i ){
		LUA_PUSH_VALUE( number, pc[i], *p[i] )
	}
	return 1;
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
template<> void initValue( s4ub& ret ) { ret = s4ub(); }

template <>
int pushToLua( lua_State* L, s4ub const& val )
{
	const GLubyte* p[4] = { &val.r, &val.g, &val.b, &val.a };
	const char* pc[4] = { "r", "g", "b", "a" };
	lua_newtable(L);
	for( int i = 0; i < 4; ++i ){
		LUA_PUSH_VALUE( number, pc[i], *p[i] )
	}
	return 1;
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


// s3fc
template <>
int pushToLua( lua_State* L, s3fc const& val )
{
	const float* p[3] = { &val.r, &val.g, &val.b };
	const char* pc[3] = { "r", "g", "b" };
	lua_newtable(L);
	for( int i = 0; i < 3; ++i ){
		LUA_PUSH_VALUE( number, pc[i], *p[i] )
	}
	return 1;
}


template<>
void getFromLua( lua_State* L, int idx, s3fc& val )
{
	float* r[3] = { &val.r, &val.g, &val.b };
	for( int i = 0; i < 3; ++i ){
		LUA_GET_TABLE_VALUE( i + 1, *r[i] )
	}
}


template<>
bool CHINP_TESTER<s3fc>(lua_State* L, int idx)
{
	if( lua_istable( L, idx ) ){
		for( int i = 1; i < 4; ++i ){
			lua_pushnumber( L, i );
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

