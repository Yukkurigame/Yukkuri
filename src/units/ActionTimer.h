/*
 * ActionTimer.h
 *
 *  Created on: 18.07.2012
 *
 */

#ifndef ACTIONTIMER_H_
#define ACTIONTIMER_H_

class Unit;

#include "types.h"
#include "Prototypes.h"
#include "3rdparty/timer/ITimerEventPerformer.h"

class IActionTimer : public ITimerEventPerformer
{
public:
	Unit* obj;
	LuaRegRef func;

	void OnTimer( InternalTimerEvent& ev );
	void OnTimerEventDestroy( const InternalTimerEvent& ev ) {};

	IActionTimer( Unit* u, LuaRegRef f ) : obj(u), func(f) {};
};


struct ActionTimer {
	const Frame* frame;
	IActionTimer* action;
	ActionTimer* next;
	UINT timerId;
	ActionTimer( const Frame* f, IActionTimer* a, ActionTimer* n ) : frame(f), action(a), next(n) {}
	~ActionTimer() {
		delete action, action = NULL;
	}
};


#endif /* ACTIONTIMER_H_ */
