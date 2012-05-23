/*
 * ThreadManager.h
 *
 *  Created on: 22.05.2012
 *
 */

#ifndef APITHREADMANAGER_H_
#define APITHREADMANAGER_H_


#include "Lua.h"



namespace ThreadApi
{

	int newThread( lua_State* L );
	int stopThread( lua_State* L );
	int stopAllThreads( lua_State* L );
	int wait( lua_State* L );
	int resume( lua_State* L );

	const luaL_reg methods[] = {
			LUALIBFIELD( newThread )
			LUALIBFIELD( stopThread )
			LUALIBFIELD( stopAllThreads )
			LUALIBFIELD( wait )
			LUALIBFIELD( resume )
			LUALIBEND
	};

}


#endif /* APITHREADMANAGER_H_ */
