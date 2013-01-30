/*
 * Camera.h
 *
 *  Created on: 23.05.2012
 *
 */

#ifndef APICAMERA_H_
#define APICAMERA_H_

#include "scripts/Lua.h"


namespace CameraApi {

	int position( lua_State* L );
	int moveTo( lua_State* L );
	int setMode( lua_State* L );
	int setTarget( lua_State* L );
	int getTarget( lua_State* L );

	const luaL_reg methods[] = {
		LUALIBFIELD( position )
		LUALIBFIELD( moveTo )
		LUALIBFIELD( setMode )
		LUALIBFIELD( getTarget )
		LUALIBFIELD( setTarget )
		LUALIBEND
	};
}



#endif /* APICAMERA_H_ */
