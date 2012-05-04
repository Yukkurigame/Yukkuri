/*
 * Prototypes.cpp
 *
 *  Created on: 29.04.2012
 *
 */
#include "Prototypes.h"
#include "debug.h"
#include <cstdlib>

extern long sdl_time;

Action::~Action( )
{
	for( UINT i = 0; i < framesCount; ++i )
		free(frames[i].txt_param);
	free(frames);
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

void ActionManager::setAction( std::string aname )
{
	action = NULL;
	if( !loaded )
		return;
	actionname = aname;
	action = proto->getAction( aname );
	frame = -1;
	done = false;
	forceParent = false;
}

void ActionManager::setParentAction( std::string aname )
{
	if( !loaded )
		return;
	// Load the same action from parent if it was called without name
	if( aname == "" )
		aname = actionname;
	else
		actionname = aname;
	action = proto->getParentAction( aname );
	if( action == NULL )
		Debug::debug( Debug::PROTO, "Action " + aname + " not found in parent prototypes of " +
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

	if( frame < 0 || now - lastTick >= action->frames[frame].duration ){
		lastTick = now;

		if( frame >= (int)action->framesCount - 1 ){
			done = true;
			if( forceParent ){
				restoreState();
				forceParent = false;
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
