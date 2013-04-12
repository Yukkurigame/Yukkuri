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
	int generateRegion( lua_State* L );
	int loadRegion( lua_State* L );
	int imageName( lua_State* L );
	int latitude( lua_State* L );
	int longitude( lua_State* L );

	const luaL_reg methods[] = {
		LUALIBFIELD( active )
		LUALIBFIELD( generateRegion )
		LUALIBFIELD( loadRegion )
		LUALIBFIELD( imageName )
		LUALIBFIELD( latitude )
		LUALIBFIELD( longitude )
		LUALIBEND
	};

}


#endif /* REGIONAPI_H_ */
