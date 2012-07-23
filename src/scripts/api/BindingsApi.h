/*
 * Bindings.h
 *
 *  Created on: 24.06.2012
 *
 */

#ifndef BINDINGSAPI_H_
#define BINDINGSAPI_H_

#include "scripts/Lua.h"


namespace BindingsApi
{

	int set( lua_State* L );
	int getName( lua_State* L );
	int setReciever( lua_State* L );

	const luaL_reg methods[] = {
		LUALIBFIELD( set )
		LUALIBFIELD( getName )
		LUALIBFIELD( setReciever )
		LUALIBEND
	};
}


#endif /* BINDINGSAPI_H_ */
