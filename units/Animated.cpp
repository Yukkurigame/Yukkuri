/*
 * Animated.cpp
 *
 *  Created on: 20.07.2010
 */
#include "Animated.h"
#include "Define.h"
#include "config.h"

AnimatedUnit::AnimatedUnit( )
{
	extern MainConfig conf;
	TimePassed = 0;
	TimeLimit = conf.actionInterval;
}

void AnimatedUnit::update( const int& dt )
{
	Unit::update( dt );
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
