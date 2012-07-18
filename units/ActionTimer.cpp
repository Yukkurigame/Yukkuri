/*
 * ActionTimer.cpp
 *
 *  Created on: 18.07.2012
 *
 */

#include "units/ActionTimer.h"
#include "scripts/LuaScript.h"
//#include "3rdparty/CUDataUser.h"
#include "units/Unit.h"

extern LuaScript* luaScript;

void IActionTimer::OnTimer( InternalTimerEvent& ev )
{
	obj->pushUData( luaScript->getState() );
	luaScript->ExecChunkFromReg( func, 1 );
}

