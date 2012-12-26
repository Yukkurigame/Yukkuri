/*
 * physics.h
 *
 *  Created on: 18.07.2012
 *
 */

#ifndef PHYSICS_H_
#define PHYSICS_H_

#include "chipmunk/chipmunk.h"

/*  Physics layers
 *
 *   1 - Base
 *   2 - Plant
 *   4 - Entity
 *
 */



struct Material {
	cpFloat density;  // kg/m^3
	Material() : density(2000.0) {}
};


enum  PhysObectType { potNone = 0, potCircle, potQuad, potRectangle, potPolygon, potLast };

enum PhysParamType { pptMat, pptRadius, pptSides };



struct PhysObject {
	Material mat;
	enum  PhysObectType type;
	cpFloat mass;	// kg, For quick calculation
	cpFloat radius; // mm, For circles
	cpVect sides;	// mm, For rectangles
	cpFloat scale_factor;
	//cpVect frictionForce; // N

	PhysObject() : type(potCircle), mass(1.0), radius(1.0), sides(), scale_factor(1.0) {}
				// frictionForce() {}

	void calc_mass( );
	void scale( cpFloat factor );
};


namespace Phys
{
	extern cpSpace* space;
	extern float friction;
	extern float g;
	void init();
	void clean();
}


#endif /* PHYSICS_H_ */
