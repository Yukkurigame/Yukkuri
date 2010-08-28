/*
 * Spawner.h
 *
 *  Created on: 26.08.2010
 */

#ifndef SPAWNER_H_
#define SPAWNER_H_

#include "unitmanager.h"
#include "Define.h"

//I think, this class is temporary.

class Spawner
{
public:
	Spawner() {};
	~Spawner() {};

	void Spawn();

private:
	void getPosition( float* x, float* y );
};


#endif /* SPAWNER_H_ */
