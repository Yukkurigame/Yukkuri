/*
 * PathsApi.h
 *
 *  Created on: 25.07.2012
 *
 */

#ifndef PATHSAPI_H_
#define PATHSAPI_H_

#include "scripts/Lua.h"


namespace PathsApi
{

	int app( lua_State* L );

	const luaL_reg methods[] = {
		LUALIBFIELD( app )
		LUALIBEND
	};

}


#endif /* PATHSAPI_H_ */
