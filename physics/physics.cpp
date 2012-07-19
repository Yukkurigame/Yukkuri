/*
 * physics.cpp
 *
 *  Created on: 19.07.2012
 *
 */

#include "physics/physics.h"

namespace Phys
{
	cpSpace* space = NULL;
}


void PhysObject::calc_mass( ){
	switch( type ){
		case potCircle:
			mass = mat.density * M_PI * radius * radius;
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
}

void PhysObject::scale( cpFloat factor )
{
	cpFloat f;
	if( factor < 0.1 )
		factor = 0.1;
	f = factor / scaleFactor;
	radius *= f;
	sides.x *= f;
	sides.y *= f;
	calc_mass();
	scaleFactor = factor;
}
