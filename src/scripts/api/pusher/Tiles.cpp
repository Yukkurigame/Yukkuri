/*
 * Region.cpp
 *
 *  Created on: 04.02.2013
 */

#include "3rdparty/LuaPusher.h"
#include "scripts/Lua.h"
#include "map/Tiles.h"
#include "graphics/render/Textures.h"
#include "string.h"


// TileInfo
template <>
int pushToLua( lua_State* L, TileInfo const& val )
{
	TextureInfo* tex = Textures::get_pointer( val.texture );

	lua_newtable(L);
	LUA_PUSH_VALUE( number, "picture", val.picture )
	LUA_PUSH_VALUE( number, "passability", val.passability )
	LUA_PUSH_VALUE( string, "image", tex->id )
	LUA_PUSH_VALUE( string, "id", val.id )

	return 1;
}


template<>
void getFromLua( lua_State* L, int idx, TileInfo& val )
{
	char* image = NULL;
	LUA_GET_VALUE( "image", image )
	val.texture = Textures::get_by_name( image );
	free( image );

	LUA_GET_VALUE( "passability", val.passability )
	LUA_GET_VALUE( "picture", val.picture )
	LUA_GET_VALUE( "id", val.id );
}

template<>
bool CHINP_TESTER<TileInfo>(lua_State* L, int idx)
{
	if( lua_istable( L, idx ) ){
		lua_pushstring( L, "id" );
		lua_gettable( L, -2 );
		if( lua_isstring( L, -1 ) && strlen( lua_tostring( L, -1 ) ) ){
			lua_pop( L, 1 );
			return true;
		}
		lua_pop( L, 1 );
	}
	return false;
}

/*
// RegionTile
template<> void initValue( RegionTile& ret ) {
	ret.id = NULL;
	ret.postion = s2f();
}

template <>
int pushToLua( lua_State* L, RegionTile const& val )
{
	lua_newtable(L);
	LUA_PUSH_VALUE( number, "x", val.postion.x )
	LUA_PUSH_VALUE( number, "y", val.postion.y )
	LUA_PUSH_VALUE( string, "tile", val.id )

	return 1;
}


template<>
void getFromLua( lua_State* L, int idx, RegionTile& val )
{
	// FIXME: May be leak
	val.id = NULL;
	LUA_GET_VALUE( "tile", val.id )
	LUA_GET_VALUE( "x", val.postion.x )
	LUA_GET_VALUE( "y", val.postion.y )
}


template<>
bool CHINP_TESTER<RegionTile>(lua_State* L, int idx)
{
	if( lua_istable( L, idx ) ){
		lua_pushstring( L, "tile" );
		lua_gettable( L, -2 );
		if( lua_isstring( L, -1 ) && strlen( lua_tostring( L, -1 ) ) ){
			lua_pop( L, 1 );
			return true;
		}
		lua_pop( L, 1 );
	}
	return false;
}

*/
