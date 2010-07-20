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

	bool Create( );

	bool isEdible( ) { return true; }

	float getNutritive( ) {
		return nutritive;
	}

	void update( const int& );

	void hit( float damage );

private:
	float hp;
	float nutritive;

};

#endif /* PLANTS_H_ */
