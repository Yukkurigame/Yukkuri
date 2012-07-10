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


void ActionManager::setProto( Proto* p )
{
	proto = p;
	if( proto != NULL )
		loaded = true;
}

void ActionManager::setAction( std::string aname, bool force )
{
	action = NULL;
	if( !loaded )
		return;
	actionname = aname;
	action = proto->getAction( aname );
	frame = -1;
	done = false;
	forceParent = force;
}

void ActionManager::setParentAction( const char* aname )
{
	if( !loaded )
		return;
	// Load the same action from parent if it was called without name
	if( aname != NULL )
		actionname = std::string(aname);
	action = proto->getParentAction( actionname );
	if( action == NULL )
		Debug::debug( Debug::PROTO, "Action " + actionname + " not found in parent prototypes of " +
				proto->name + " prototype.\n" );
	frame = -1;
	done = false;
	forceParent = true;
}


bool ActionManager::nextFrame( )
{
	if( done || action == NULL )
		return true;

	UINT now = sdl_time;

	Frame& f = action->frames[frame];

	if( frame < 0 || now - lastTick >= f.duration || f.repeat > now ){

		lastTick = now;

		if( frame >= (int)action->framesCount - 1 ){
			if( !looped )
				done = true;
			if( forceParent ){
				forceParent = false;
				restoreState();
				if( frame >= (int)action->framesCount - 1 ){
					done = true;
					return true;
				}
			}else
				return true;
		}

		frame++;

	}else{
		return true;
	}

	return false;
}



void ActionManager::saveState()
{
	stateStack.push( ActionManagerState( frame, actionname, sdl_time - lastTick ) );
}

// This function returns state flag depends of execution
// If state is not restored state flag should remain true
void ActionManager::restoreState()
{
	if( stateStack.empty() )
		return;
	ActionManagerState s = stateStack.top();
	setAction( s.currentAction );
	frame = s.currentFrame;
	lastTick = sdl_time - s.prevActionTickDiff;
	stateStack.pop();
	return;
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
		if( fr.param_types[i] != setp ){
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
