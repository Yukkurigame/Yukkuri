/*
 * UnitManager.h
 *
 *  Created on: 19.05.2012
 *
 */

#ifndef UNITMANAGER_H_
#define UNITMANAGER_H_

#include "Lua.h"

#define LUALIBFIELD( f ) { #f, f },
#define LUALIBEND {NULL, NULL}

namespace UMApi {

	int createUnit( lua_State* L );
	int deleteUnit( lua_State* L );
	int getUnit( lua_State* L );
	int getPlayer( lua_State* L );
	int getUnitsSize( lua_State* L );


	const luaL_reg methods[] = {
		LUALIBFIELD( createUnit )
		LUALIBFIELD( deleteUnit )
		LUALIBFIELD( getUnit )
		LUALIBFIELD( getPlayer )
		LUALIBFIELD( getUnitsSize )
		LUALIBEND
	};
}


#endif /* UNITMANAGER_H_ */
