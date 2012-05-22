/*
 * TimerManager.h
 *
 *  Created on: 22.05.2012
 *
 * Origin: iiChantra
 *
 */

#ifndef TIMERMANAGER_H_
#define TIMERMANAGER_H_


#include "types.h"
#include "3rdparty/timer/ITimerEventPerformer.h"


namespace Timer {

	UINT AddEvent( UINT dt, LuaRegRef action, UINT period, UINT maxCalls );
	UINT AddEvent( UINT dt, LuaRegRef action );
	void AddThreadTimerEvent( UINT dt, LuaRegRef action, bool pausable );
	int AddInternalTimerEvent( ITimerEventPerformer* performer, UINT dt, UINT period, UINT max_Calls, bool periodic, bool pausable );
	void DeleteAllEvents( );
	void DeleteAllThreadEvents( );
	int DeleteTimerEvent( LuaRegRef action );
	bool DeleteTimerEventById( UINT id );
	void ProcessTimerEvents( );

}

#endif /* TIMERMANAGER_H_ */
