/*
 * Animated.cpp
 *
 *  Created on: 20.07.2010
 */
#include "Animated.h"
#include "Define.h"

AnimatedUnit::AnimatedUnit( )
{
	TimePassed = 0;
	TimeLimit = ACTION_INTERVAL;
}

void AnimatedUnit::update( const int& dt )
{
	TimePassed += dt;
	if( TimePassed >= TimeLimit ){
		TimePassed = 0;
		takeAction();
	}
}

void AnimatedUnit::setTimeLimit( int time )
{
	TimeLimit = time;
}

void AnimatedUnit::takeAction( )
{
	if( !getUnitImage()->clr->isMax() )
		getUnitImage()->clr->set(255);
}
