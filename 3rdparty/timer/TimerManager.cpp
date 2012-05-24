/*
 * TimerManager.cpp
 *
 *  Created on: 22.05.2012
 *
 * Origin: iiChantra
 *
 */

#include "3rdparty/timer/TimerManager.h"
#include "3rdparty/timer/TimerEvent.h"
#include "3rdparty/timer/InternalTimerEvent.h"
#include "scripts/LuaThread.h"

#include <list>
#include "hacks.h"
#include "assert.h"

extern long sdl_time;
extern long last_timerevent_tick;

#define PAUSE 0

//////////////////////////////////////////////////////////////////////////

UINT TimerEvent::idCounter;


typedef std::list<TimerEvent*>::iterator TEvIter;


namespace {

	//////////////////////////////////////////////////////////////////////////
	// Список событий
	std::list <TimerEvent*> timerEvents;
	UINT nbPausableEvents = 0;

	// События в списке всегда отсортированы так, что в начале находятся
	// те, что наступят раньше всего. Таким образом, достаточно смотреть только
	// первое событие, если оно не наступило, то другие тоже еще не наступили.


	// Добавление события в массив createdEvents и сортировка массива
	void AddTimerEvent( TimerEvent* te ){
		if (te->IsPausable())
			nbPausableEvents++;

		// Ищем событие, которое наступит позже, чем добавляемое.
		FOREACHIT( timerEvents ){
			if (te->execTime < (*it)->execTime){
				// Вставляем перед найденным событием
				timerEvents.insert(it, te);
				return;
			}
		}
		// Событие произойдет позже всех, добавляем в конец
		timerEvents.push_back(te);
	}

}



// Добавление периодического события
// dt - время, черз которое событие сработает
// action - номер скрипта в реестре
// period - период повторения после первого срабатываения
// maxCalls - количество срабатываний
UINT Timer::AddEvent( UINT dt, LuaRegRef action, UINT period, UINT maxCalls )
{
	TimerEvent* te = new TimerEvent();

	te->action = action;
	te->execTime = sdl_time + dt;

	te->SetPeriodic();
	te->period = period;
	te->maxCalls = maxCalls;

	AddTimerEvent( te );
	return 1;
}


// Добавление события
// dt - время, черз которое событие сработает
// action - номер скрипта в реестре
UINT Timer::AddEvent( UINT dt, LuaRegRef action )
{
	TimerEvent* te = new TimerEvent();

	te->action = action;
	te->execTime = sdl_time + dt;

	AddTimerEvent(te);

	return 1;
}


// Добавлеят событие восстановления приостановленного потока
void Timer::AddThreadTimerEvent( UINT dt, LuaRegRef action, bool pausable )
{
	TimerEvent* te = new TimerEvent();
	te->action = action;
	te->execTime = sdl_time + dt;
	te->SetThreadResume();
	if( pausable )
		te->SetPausable();

	AddTimerEvent(te);
}


int Timer::AddInternalTimerEvent( ITimerEventPerformer* performer, UINT dt, UINT period, UINT max_Calls, bool periodic, bool pausable )
{
	InternalTimerEvent* te = new InternalTimerEvent();
	te->performer = performer;
	te->execTime = sdl_time + dt;
	if( periodic ){
		te->SetPeriodic();
		te->period = period;
		if( max_Calls == 0 )
			te->SetEndless();
		te->maxCalls = max_Calls;
	}
	if( pausable )
		te->SetPausable();

	AddTimerEvent(te);

	return te->id;
}


// Удаление события таймера с заданным action
int Timer::DeleteTimerEvent(LuaRegRef action)
{
	// Посик ближайшего подходящего события и его удаление
	TimerEvent* t = NULL;
	TEvIter it;
	FOREACHIT( timerEvents ){
		t = *it;
		if( t->action == action )
			break;
	}

	if( it != timerEvents.end() ){
		timerEvents.erase(it);
		if( t->IsPausable() ){
			assert(nbPausableEvents);
			nbPausableEvents--;
		}
		delete t, t = NULL;
		return 1;
	}
	return 0;
	// TODO: возможно стоит обходить весь список, а не удалять первое ближайшее
	// подходящее событие. Вот код из библиотеки для удаелния нескольких элементов.
	//iterator _Last = end();
	//for (iterator _First = begin(); _First != _Last; )
	//	if ((*_First)->)
	//		_First = erase(_First);
	//	else
	//		++_First;
}

// Удаление события таймера с заданным action
bool Timer::DeleteTimerEventById(UINT id)
{
	// Посик ближайшего подходящего события и его удаление
	TimerEvent* t = NULL;
	TEvIter it;
	FOREACHIT( timerEvents ){
		t = *it;
		if( t->id == id )
			break;
	}

	if( it != timerEvents.end() ){
		timerEvents.erase(it);
		if( t->IsPausable() ){
			assert(nbPausableEvents);
			nbPausableEvents--;
		}
		delete t, t = NULL;
		return true;
	}
	return false;
}

// Удаляет все события запуска тредов
void Timer::DeleteAllThreadEvents()
{
	TimerEvent* t = NULL;
	TEvIter it;
	for(it = timerEvents.begin(); it != timerEvents.end(); ){
		t = *it;
		if (t->IsThreadResume()){
			delete t, t = NULL;
			it = timerEvents.erase(it);
		}else{
			++it;
		}
	}
}

// Удаляет все события из массива
void Timer::DeleteAllEvents()
{
	FOREACHIT( timerEvents ){
		delete *it, *it = NULL;
	}

	timerEvents.clear();
}


// Обработка событий таймера
// Вызывается минимум через каждые TIMEREVENTTICK милисекунд
void Timer::ProcessTimerEvents()
{
	if( timerEvents.empty() )
		return;

	TimerEvent* first = NULL;

	if( nbPausableEvents && PAUSE )
	{
		// К execTime каждого приостановленного события добавится shift
		UINT shift = sdl_time - last_timerevent_tick;


		std::list<TimerEvent*>::reverse_iterator rit = timerEvents.rbegin();
		std::list<TimerEvent*>::reverse_iterator prev_rit = rit;
		std::list<TimerEvent*>::reverse_iterator temp_rit = rit;

		// Поиск приостановленных событий с конца
		for( ; rit != timerEvents.rend(); prev_rit = rit, rit++ ){
			if (!(*rit)->IsPausable())
				continue;

			first = *rit;
			first->execTime += shift;

			temp_rit = rit;
			// Перенос события так, чтобы список оставался отсортированным
			while((*prev_rit)->execTime < first->execTime){
				*temp_rit = *prev_rit;
				*prev_rit = first;
				if (prev_rit == timerEvents.rbegin())
					break;
				temp_rit = prev_rit;
				prev_rit--;
			}
		}
	}

	while( !timerEvents.empty() ){
		first = *(timerEvents.begin());	// Указатель на ближайшее событие

		if (first->execTime > sdl_time)
			break;							// Время еще не пришло

		assert( ( !first->IsPausable() && PAUSE ) || !PAUSE );

		first->ExecEvent();

		if( timerEvents.empty() || first != *(timerEvents.begin()) ){
			// Первое событие уже удалили где-то в его обработчике
			continue;
		}

		if( first->IsPeriodic() ){
			first->calls++;
			if( !first->IsEndless() && first->calls >= first->maxCalls - 1 ){
				// Предпоследний вызов периодического события
				// В следующий раз оно уже вызовется как обыкновенное и исчезнет
				first->ClearPeriodic();
			}

			first->execTime = sdl_time + first->period;
			// Удаляем периодическое событие и опять добавляем его в массив
			timerEvents.pop_front();
			AddTimerEvent(first);
		}else{
			// Удаляем отработавшее событие
			timerEvents.pop_front();
			delete first, first = NULL;
		}
	}

}


// Исполняет настпуившее событие.
void TimerEvent::ExecEvent()
{
	if( this->IsThreadResume() ){
		threadsManager::ProcessThread(this->action);
	}else{
		extern LuaScript* luaScript;
		luaScript->ExecChunkFromReg( this->action );
	}
}
