/*
 * ITimerEventPerformer.h
 *
 *  Created on: 22.05.2012
 *
 * Origin: iiChantra
 *
 */

#ifndef ITIMEREVENTPERFORMER_H_
#define ITIMEREVENTPERFORMER_H_

class InternalTimerEvent;

// Интерфейс для классов, которые хотят подписаться на события таймера.
class ITimerEventPerformer
{
public:
	virtual ~ITimerEventPerformer() {}
	// Возникает при наступлении событии таймера.
	virtual void OnTimer( InternalTimerEvent& ev ) = 0;
	// Возникает при удалении TimerEvent
	virtual void OnTimerEventDestroy( const InternalTimerEvent& ev ) = 0;
};



#endif /* ITIMEREVENTPERFORMER_H_ */
