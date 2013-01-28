/*
 * TextureProxy.cpp
 *
 *  Created on: 28.01.2013
 *
 */


#include "3rdparty/LuaPusher.h"
#include "scripts/Lua.h"
#include "graphics/GraphicsTypes.h"


// TextureProxy

template <>
int pushToLua( lua_State* L, TextureProxy const& val )
{
	const int* p[4] = { &val.abs.width, &val.abs.height, &val.rows, &val.cols };
	const char* pc[4] = { "width", "height", "rows", "columns" };

	lua_newtable(L);
	LUA_PUSH_VALUE( string, "id", val.id.c_str() );
	LUA_PUSH_VALUE( string, "image", val.image.c_str() );

	for( int i = 0; i < 4; ++i ){
		LUA_PUSH_VALUE( number, pc[i], *p[i] );
	}
	LUA_PUSH_VALUE( number, "offsetx", val.offset.x );
	LUA_PUSH_VALUE( number, "offsety", val.offset.y );

	return 1;
}

template<>
TextureProxy const getFromLua( lua_State* L, int idx )
{
	TextureProxy ret;
	getFromLua( L, idx, ret );
	return ret;
}

template<>
void getFromLua( lua_State* L, int idx, TextureProxy& val )
{
	const int* p[4] = { &val.abs.width, &val.abs.height, &val.rows, &val.cols };
	const char* pc[4] = { "width", "height", "rows", "columns" };

	lua_newtable(L);
	LUA_GET_VALUE( "id", val.id );
	LUA_GET_VALUE( "image", val.image );

	for( int i = 0; i < 4; ++i ){
		LUA_GET_VALUE( pc[i], *p[i] );
	}
	LUA_GET_VALUE( "offsetx", val.offset.x );
	LUA_GET_VALUE( "offsety", val.offset.y );

}

template<>
bool CHINP_TESTER<TextureProxy>(lua_State* L, int idx)
{
	if( lua_istable( L, idx ) ){
		lua_pushstring( L, "type" );
		lua_gettable( L, -2 );
		if( lua_isstring( L, -1 ) && !strcmp( "sprite", lua_tostring( L, -1 ) ) ){
			lua_pop( L, 1 );
			return true;
		}
		lua_pop( L, 1 );
	}
	return false;
}




