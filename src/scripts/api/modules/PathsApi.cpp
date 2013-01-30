/*
 * PathsApi.cpp
 *
 *  Created on: 26.07.2012
 *
 */

#include "scripts/api/modules/PathsApi.h"
#include "misc.h"


int PathsApi::app( lua_State* L ){
	lua_pushstring( L, Paths::app() );
	return 1;
}

