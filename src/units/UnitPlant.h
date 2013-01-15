/*
 * Plants.h
 *
 *  Created on: 18.07.2010
 */

#ifndef PLANTS_H_
#define PLANTS_H_

#include "units/UnitStatic.h"

class Plant: public UnitStatic
{
protected:
	virtual CUData* createUData();
};

#endif /* PLANTS_H_ */
