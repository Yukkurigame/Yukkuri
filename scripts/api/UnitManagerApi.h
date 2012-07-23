/*
 * UnitManager.h
 *
 *  Created on: 19.05.2012
 *
 */

#ifndef APIUNITMANAGER_H_
#define APIUNITMANAGER_H_

#include "scripts/Lua.h"


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


#endif /* APIUNITMANAGER_H_ */
