/*
 * physics.cpp
 *
 *  Created on: 19.07.2012
 *
 */

#include "units/physics/physics.h"
#include "units/physics/handlers.h"
#include "units/Unit.h"
#include <cmath>

namespace Phys
{
	cpSpace* space = NULL;
	float friction = 1.7;
	float g = 9.8;

}




void PhysObject::calc_mass( ){
	switch( type ){
		case potCircle:
			mass = mat.density * M_PI * pow(radius / 1000, 3) * 4.0 / 3.0;
			break;
		case potQuad:
		case potRectangle:
			mass = mat.density * sides.x * sides.y;
			break;
		case potPolygon:
			//Not supported yet
		case potNone:
		default:
			mass = 1.0;
			break;
	}
	//frictionForce.x = frictionForce.y = mass * Phys::g * Phys::friction;
}

void PhysObject::scale( cpFloat factor )
{
	cpFloat f;
	if( factor < 0.1 )
		factor = 0.1;
	f = factor / scale_factor;
	radius *= f;
	sides.x *= f;
	sides.y *= f;
	calc_mass();
	scale_factor = factor;
}

void Phys::init()
{
	// Create physics space;
	space = cpSpaceNew();

	cpSpaceSetDamping( space, 0.0 );

	cpSpaceAddCollisionHandler( space, utLast, utPlant,
			DynamiUint_begin, NULL, NULL, DynamiUint_separate, NULL );
	cpSpaceAddCollisionHandler( space, utLast, utEntity,
		DynamiUint_begin, NULL, NULL, DynamiUint_separate, NULL );
	cpSpaceAddCollisionHandler( space, utLast, utPlayer,
			DynamiUint_begin, NULL, NULL, DynamiUint_separate, NULL );
	cpSpaceAddCollisionHandler( space, utLast, utCorpse,
				DynamiUint_begin, NULL, NULL, DynamiUint_separate, NULL );
}

void Phys::clean()
{
	// Free physics engine
	cpSpaceFree( Phys::space );
}

