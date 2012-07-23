/*
 * TimerEvent.h
 *
 *  Created on: 22.05.2012
 *
 * Origin: iiChantra
 *
 */

#ifndef TIMEREVENT_H_
#define TIMEREVENT_H_

#include "scripts/LuaScript.h"
#include "types.h"


class TimerEvent
{
	static UINT idCounter;

public:
	UINT id;					// Возможно будет использоваться для управления из скрипта.

	LuaRegRef action;           // Ссылка на реестр луа

	long execTime;				// Время наступления события

	UINT period;				// Период периодического события
	UINT maxCalls;				// Количество срабатыванй

	UINT calls;					// Текущее количество срабатываний

	UINT flags;
	// 1 - threadResume				данное событие - запуск приостановленной coroutine
	// 2 - periodic					событие переодическое
	// 4 - endless					event is endless and does not ended
	// 8 - pausable					на время игровой паузы прекращается обработка этого события, его время "замораживается"
									// и продолжится после снятия игровой паузы

	inline BYTE IsThreadResume()	{ return flags & 1; }
	inline void SetThreadResume()	{ flags |= 1; }
	inline void ClearThreadResume()	{ flags &= ~1; }

	inline BYTE IsPeriodic()	{ return flags & 2; }
	inline void SetPeriodic()	{ flags |= 2; }
	inline void ClearPeriodic()	{ flags &= ~2; }

	inline BYTE IsEndless()	{ return flags & 4; }
	inline void SetEndless()	{ flags |= 4; }
	inline void ClearEndless()	{ flags &= ~4; }

	inline BYTE IsPausable()	{ return flags & 8; }
	inline void SetPausable()	{ flags |= 8; }
	inline void ClearPausable()	{ flags &= ~8; }

	virtual void ExecEvent();

	TimerEvent( ){
		id = ++idCounter;
		action = LUA_NOREF;
		execTime = 0;
		flags = 0;
		period = 0;
		maxCalls = 0;
		calls = 0;
	}

	virtual ~TimerEvent( ){
		if( !IsThreadResume() ){
			extern LuaScript* luaScript;
			luaScript->RemoveFromRegistry(action);
		}
	}

};


#endif /* TIMEREVENT_H_ */
