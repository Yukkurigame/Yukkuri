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
	int ReadDirectory( lua_State* L );
	int GetCWD( lua_State* L );

	int Debug( lua_State* L );

}

#endif /* API_H_ */
