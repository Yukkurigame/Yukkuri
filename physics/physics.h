/*
 * physics.h
 *
 *  Created on: 18.07.2012
 *
 */

#ifndef PHYSICS_H_
#define PHYSICS_H_

#include "chipmunk/chipmunk.h"

/*	Physics layers
 *
 * 	1 - Base
 * 	2 - Plant
 * 	4 - Entity
 *
 */



struct Material {
	cpFloat density;
	Material() : density(50.0) {}
};


enum  PhysObectType { potNone = 0, potCircle, potQuad, potRectangle, potPolygon, potLast };

enum PhysParamType { pptMat, pptType, pptRadius, pptSides };

struct PhysObject {
	Material mat;
	enum  PhysObectType type;
	cpFloat mass;	// For quick calculation
	cpFloat radius; // For circles
	cpVect sides;	// For rectangles
	cpFloat scaleFactor;

	PhysObject() : type(potCircle), mass(1.0), radius(1.0), sides(), scaleFactor(1.0) {}

	void calc_mass( );
	void scale( cpFloat factor );
};


namespace Phys
{
	extern cpSpace* space;
}


#endif /* PHYSICS_H_ */
