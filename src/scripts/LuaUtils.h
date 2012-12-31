/*
 * LuaUtils.h
 *
 *  Created on: 31.12.2012
 */

#ifndef LUAUTILS_H_
#define LUAUTILS_H_

#include "types.h"

#define OBJECT_FROM_LUA( i, id_func, type, ret )			\
if( lua_isnumber( L, i ) ){									\
	UINT id = static_cast<UINT>( lua_tointeger( L, i ) );	\
	ret = id_func( id );									\
}else if( lua_isuserdata( L, i ) )							\
	ret = reinterpret_cast<type>( getUserData( L, i ) );


#endif /* LUAUTILS_H_ */
