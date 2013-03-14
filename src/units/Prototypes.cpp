/*
 * Prototypes.cpp
 *
 *  Created on: 29.04.2012
 *
 */
#include "Prototypes.h"
#include <cstdlib>
#include <stdarg.h>

#include "hacks.h"
#include "safestring.h"
#include "debug.h"

extern long sdl_time;

namespace {
	std::map<std::string, int> action_names;
	int action_names_counter = 0;
}


Action::~Action( )
{
	// FIXME: leak
	//free(frames);
}


int Action::getId( std::string name )
{
	std::map< std::string, int >::iterator a = action_names.find( name );
	if( a != action_names.end() )
		return a->second;
	action_names[name] = ++action_names_counter;
	return action_names_counter;
}

std::string Action::getName( int id )
{
	FOREACHIT1( action_names ){
		if( it->second == id )
			return it->first;
	}
	return "";
}


Action* Proto::getAction( int id )
{
	std::map< int, Action >::iterator a = Actions.find( id );
	if( a != Actions.end() )
		return &(a->second);
	else
		return getParentAction( id );
}


Action* Proto::getParentAction( int id )
{
	if( !parent )
		return NULL;
	std::map< int, Action >::iterator faction = parent->Actions.find( id );
	if( faction == parent->Actions.end() )
		return parent->getParentAction( id );
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

void ActionManager::setAction( int id )
{
	if( !loaded )
		return;
	if( id ){
	//	printf("Setup action %s\n", Action::getName(id).c_str());
		action = NULL;
		action = proto->getAction( id );
	} //else
	//	printf("Reset action %s\n", Action::getName(action->id).c_str());
	frame = -1;
	done = false;
}

void ActionManager::setParentAction( int id )
{
	if( !loaded || proto->parent == NULL )
		return;
	// Load the same action from parent if it was called without name
	if( !id && action != NULL )
		id = action->id;
	if( action && action->id == id )
		proto = proto->parent;
	action = proto->getAction( id );
	if( action == NULL )
		Debug::debug( Debug::PROTO,
				"Action %d not found in parent prototypes of %s prototype.\n",
				id, proto->name.c_str() );
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
			Debug::debug( Debug::PROTO,
					"Wrong set of parameters of frame with action %d.\n",
					fr.command );
			return false;
		}
		// TODO: warning: ‘StackElementType’ is promoted to ‘int’ when passed through ‘...’
		// note: (so you should pass ‘int’ not ‘StackElementType’ to ‘va_arg’)
		setp = (StackElementType)va_arg( mark, int );
	}
	va_end( mark );
	return true;
}
