/*
 * RegionApi.h
 *
 *  Created on: 12.12.2012
 */

#ifndef REGIONAPI_H_
#define REGIONAPI_H_

#include "scripts/Lua.h"


namespace RegionApi
{

	int active( lua_State* L );
	int loadRegion( lua_State* L );

	const luaL_reg methods[] = {
		LUALIBFIELD( active )
		LUALIBFIELD( loadRegion )
		LUALIBEND
	};

}


#endif /* REGIONAPI_H_ */