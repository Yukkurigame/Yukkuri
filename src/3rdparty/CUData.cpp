/*
 * CUData.cpp
 *
 *  Created on: 17.05.2012
 *
 * Origin: iiChantra
 *
 */

#include "3rdparty/CUData.h"
#include "3rdparty/CUDataUser.h"
#include "scripts/LuaScript.h"

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



