/*
 * Plants.h
 *
 *  Created on: 18.07.2010
 */

#ifndef PLANTS_H_
#define PLANTS_H_

#include "Animated.h"

class Plant: public AnimatedUnit
{
public:
	Plant();

	bool Create( int id );

	bool isEdible( ) { return true; }

	void update( const int& );

	void hit( float damage );

private:


};

#endif /* PLANTS_H_ */