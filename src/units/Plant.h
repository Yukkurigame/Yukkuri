/*
 * Plants.h
 *
 *  Created on: 18.07.2010
 */

#ifndef PLANTS_H_
#define PLANTS_H_

#include "Unit.h"

class Plant: public Unit
{
public:
	Plant();

	void hit( float damage );

protected:
	virtual CUData* createUData();


private:


};

#endif /* PLANTS_H_ */
