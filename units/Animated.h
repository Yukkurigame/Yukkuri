/*
 * AnimatedUnit.h
 *
 *  Created on: 18.07.2010
 */

#ifndef ANIMATED_H_
#define ANIMATED_H_

#include "Unit.h"

class AnimatedUnit: public Unit
{
public:
	AnimatedUnit();

	void update( const int& dt );
	void setTimeLimit( int time );

	virtual void takeAction( );

protected:
	virtual CUData* createUData();

private:
	int TimePassed;
	int TimeLimit;

};

#endif /* ANIMATED_H_ */
