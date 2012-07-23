/*
 * InternalTimerEvent.h
 *
 *  Created on: 22.05.2012
 *
 * Origin: iiChantra
 *
 */

#ifndef INTERNALTIMEREVENT_H_
#define INTERNALTIMEREVENT_H_

#include "3rdparty/timer/TimerEvent.h"
#include "3rdparty/timer/ITimerEventPerformer.h"


// Событие таймера для кода движка. Посылает событие таймера создавшему его объекту.
// Объект этого класса хранит указатель на объект подписавшиегося на событие, так что
// подписавшивася доложен нормально удалить событие.
class InternalTimerEvent : public TimerEvent
{
public:
	ITimerEventPerformer* performer;	// Указатель на объект, которому посылаются события.

	virtual void ExecEvent( ){
		performer->OnTimer(*this);
	}


	virtual ~InternalTimerEvent( ){
		performer->OnTimerEventDestroy(*this);
	}
};


#endif /* INTERNALTIMEREVENT_H_ */
