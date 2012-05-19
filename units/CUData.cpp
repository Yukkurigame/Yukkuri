/*
 * CUData.cpp
 *
 *  Created on: 17.05.2012
 *
 */

#include "units/CUData.h"
#include "units/CUDataUser.h"
#include "LuaScript.h"

void CUData::onGC()
{
	if(pUser){
		pUser->setUData(NULL);
		pUser = NULL;
	}
}

void CUData::clearUser()
{
	pUser = NULL;
	extern LuaScript* luaScript;
	luaScript->RemoveFromRegistry(regRef);
}



