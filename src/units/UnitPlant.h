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
public:
	void hit( float damage );

protected:
	virtual CUData* createUData();


private:


};

#endif /* PLANTS_H_ */
