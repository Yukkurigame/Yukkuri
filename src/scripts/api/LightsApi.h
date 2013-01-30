/*
 * LightsApi.h
 *
 *  Created on: 30.01.2013
 *
 */

#ifndef LIGHTSAPI_H_
#define LIGHTSAPI_H_


#include "scripts/Lua.h"


namespace LightsApi {

	int create( lua_State* L );
	int remove( lua_State* L );
	int count( lua_State* L );

	const luaL_reg methods[] = {
		LUALIBFIELD( create )
		LUALIBFIELD( remove )
		LUALIBFIELD( count )
		LUALIBEND
	};
}



#endif /* LIGHTSAPI_H_ */
