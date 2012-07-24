/*
 * handlers.h
 *
 *  Created on: 24.07.2012
 *
 */

#ifndef PHYSICS_COLLISIONS_HANDLERS_H_
#define PHYSICS_COLLISIONS_HANDLERS_H_

#include "units/physics/physics.h"


int DynamiUint_begin( cpArbiter* arb, cpSpace* space, void* unused );
void DynamiUint_separate( cpArbiter* arb, cpSpace* space, void* unused );



#endif /* PHYSICS_COLLISIONS_HANDLERS_H_ */
