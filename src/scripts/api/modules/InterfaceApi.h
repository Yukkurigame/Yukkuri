/*
 * Interface.h
 *
 *  Created on: 20.05.2012
 *
 */

#ifndef APIINTERFACE_H_
#define APIINTERFACE_H_


#include "scripts/Lua.h"


namespace IfaceApi {

	int createWidget( lua_State* L );
	int getWidget( lua_State* L );
	int getWidgetByName( lua_State* L );
	int addChild( lua_State* L );

	const luaL_reg methods[] = {
		//LUALIBFIELD( loadAllWidgets )
		//LUALIBFIELD( loadWidget )
		LUALIBFIELD( createWidget )
		LUALIBFIELD( getWidget )
		LUALIBFIELD( getWidgetByName )
		LUALIBFIELD( addChild )
		LUALIBEND
	};
}



#endif /* APIINTERFACE_H_ */
