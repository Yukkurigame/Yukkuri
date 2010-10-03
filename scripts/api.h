/*
 * api.h
 *
 *  Created on: 21.09.2010
 */

#ifndef API_H_
#define API_H_

#include "Lua.h"

namespace scriptApi
{
	int LoadConfig( lua_State* L );
	int LoadAllConfigs( lua_State* L );

	int CreateWidget( lua_State* L );
	int LoadWidget( lua_State* L );

	int CreateUnit( lua_State* L );
	int DeleteUnit( lua_State* L );
}

#endif /* API_H_ */