/*
 * Interface.h
 *
 *  Created on: 20.05.2012
 *
 */

#ifndef APIINTERFACE_H_
#define APIINTERFACE_H_


#include "Lua.h"


namespace IfaceApi {

	int loadAllWidgets( lua_State* L );
	int loadWidget( lua_State* L );
	int getWidget( lua_State* L );
	int getWidgetByName( lua_State* L );

	const luaL_reg methods[] = {
		LUALIBFIELD( loadAllWidgets )
		LUALIBFIELD( loadWidget )
		LUALIBFIELD( getWidget )
		LUALIBFIELD( getWidgetByName )
		LUALIBEND
	};
}



#endif /* APIINTERFACE_H_ */
