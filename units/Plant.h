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

	bool isEdible( ) { return true; }

	void update( const int& );

	void hit( float damage );

protected:
	virtual CUData* createUData();


private:


};

#endif /* PLANTS_H_ */
