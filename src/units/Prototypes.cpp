/*
 * Prototypes.cpp
 *
 *  Created on: 29.04.2012
 *
 */
#include "Prototypes.h"
#include <cstdlib>
#include <stdarg.h>

#include "safestring.h"
#include "debug.h"

extern long sdl_time;


Action::~Action( )
{
	// FIXME: leak
	//free(frames);
}

Action* Proto::getAction( std::string aname )
{
	std::map< std::string, Action >::iterator a = Actions.find( aname );
	if( a != Actions.end() )
		return &(a->second);
	else
		return getParentAction( aname );
}


Action* Proto::getParentAction( std::string aname )
{
	if( !parent )
		return NULL;
	std::map< std::string, Action >::iterator faction = parent->Actions.find( aname );
	if( faction == parent->Actions.end() )
		return parent->getParentAction( aname );
	return &faction->second;
}


ActionManager::~ActionManager( )
{
	FrameTimer* timer = NULL;
	while( skipTimers != NULL ){
		timer = skipTimers;
		skipTimers = skipTimers->next;
		delete timer, timer = NULL;
	}
	while( !stateStack.empty() )
		restoreState();
}

void ActionManager::setProto( Proto* p )
{
	proto = p;
	if( proto != NULL )
		loaded = true;
}

void ActionManager::setAction( const char* aname )
{
	if( !loaded )
		return;
	if( aname != NULL ){
		action = NULL;
		action = proto->getAction( aname );
	}
	frame = -1;
	done = false;
}

void ActionManager::setParentAction( const char* aname )
{
	if( !loaded || proto->parent == NULL )
		return;
	// Load the same action from parent if it was called without name
	std::string actionname = "";
	if( aname != NULL )
		actionname = std::string(aname);
	else if( action != NULL )
		actionname = action->name;
	if( action && action->name == actionname )
		proto = proto->parent;
	action = proto->getAction( actionname );
	if( action == NULL )
		Debug::debug( Debug::PROTO, "Action " + actionname + " not found in parent prototypes of " +
				proto->name + " prototype.\n" );
	frame = -1;
	done = false;
}


void ActionManager::updateTimers( const int& dt )
{
	FrameTimer* timer = skipTimers;
	while( timer != NULL ){
		timer->time -= dt;
		timer = timer->next;
	}
}


bool ActionManager::nextFrame( )
{
	if( done || action == NULL )
		return true;

new_frame: ;

	if( frame < 0 || (UINT)(sdl_time - lastTick) >= action->frames[frame].duration ){

		lastTick = sdl_time;

		if( frame >= (int)action->framesCount - 1 ){
			done = true;
			if( topStateForced() ){
				restoreState();
				// It may cause a bug,
				// When f.duration of restored frame is more than delta, controller
				// may repeat previous command that swiched the flow to this state
				goto new_frame;
			}
			return true;
		}

		frame++;

		Frame* pframe = &(action->frames[frame]);
		if( pframe->skip_on_delay ){
			FrameTimer* timer = skipTimers;
			while( timer != NULL ){
				if( timer->frame == pframe )
					break;
				timer = timer->next;
			}
			if( !timer )
				skipTimers = new FrameTimer( pframe, pframe->skip_on_delay, skipTimers );
			else if( timer->time <= 0 )
				timer->time = pframe->skip_on_delay;
			else
				goto new_frame;
		}
	}else
		return true;

	return false;
}


void ActionManager::saveState( bool forced )
{
	stateStack.push( new ActionManagerState( frame, action, proto,
			sdl_time - lastTick, forced ) );
}


// This function returns state flag depends of execution
// If state is not restored state flag should remain true
void ActionManager::restoreState()
{
	if( stateStack.empty() )
		return;
	ActionManagerState* s = stateStack.top();
	action = s->currentAction;
	proto = s->currentPrototype;
	frame = s->currentFrame;
	lastTick = sdl_time - s->prevActionTickDiff;
	done = false;
	stateStack.pop();
	delete s;
}


void ActionManager::popState()
{
	stateStack.pop();
}


bool ActionManager::checkFrameParams( const Frame& fr, int num, enum StackElementType first, ... )
{
	va_list mark;
	StackElementType setp = first;
	if( num > FRAME_PARAMS_COUNT ){
		Debug::debug( Debug::PROTO, "Wrong count of parameters to check.\n" );
		num = FRAME_PARAMS_COUNT;
	}
	va_start( mark, first );
	for( int i = 0; i < num; i++ ){
		const StackElementType& el = fr.param_types[i];
		if( !( el == setp ||
			( setp == stIntOrNone    && (el == stInt    || el == stNone) ) ||
			( setp == stStringOrNone && (el == stString || el == stNone) ) ||
			( setp == stTableOrNone  && (el == stTable  || el == stNone) )  ) ){
			Debug::debug( Debug::PROTO, "Wrong set of parameters of frame with action " +
					citoa( fr.command ) + ".\n" );
			return false;
		}
		// TODO: warning: ‘StackElementType’ is promoted to ‘int’ when passed through ‘...’
		// note: (so you should pass ‘int’ not ‘StackElementType’ to ‘va_arg’)
		setp = (StackElementType)va_arg( mark, int );
	}
	va_end( mark );
	return true;
}
