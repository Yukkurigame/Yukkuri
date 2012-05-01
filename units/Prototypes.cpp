/*
 * Prototypes.cpp
 *
 *  Created on: 29.04.2012
 *
 */
#include "Prototypes.h"

void ActionManager::setProto( Proto p )
{
	proto = p;
	loaded = true;
}



void ActionManager::setAction( std::string aname )
{
	action = NULL;
	if( !loaded )
		return;
	std::map< std::string, Action >::iterator a = proto.Actions.find( aname );
	if( a != proto.Actions.end() ){
		action = &(a->second);
	}
}



bool ActionManager::nextFrame( )
{
	if( done || action == NULL )
		return true;

	extern long sdl_time;

	UINT now = sdl_time;

	if( !lastTick ){
		lastTick = now;
		return false;
	}

	if( now - lastTick >= action->frames[frame].duration ){
		lastTick = now;

		if( frame >= action->framesCount - 1 ){
			done = true;
			return true;
		}

		frame++;

	}else{
		return true;
	}

	return false;
}

