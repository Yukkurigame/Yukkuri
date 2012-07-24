/*
 * Plant.cpp
 *
 *  Created on: 18.07.2010
 */
#include "Plant.h"


bool Plant::Create( int id, std::string proto )
{
	if( !Unit::Create( id, proto ) )
		return false;
	cpShape* shape = cpCircleShapeNew( physBody, phys.radius, cpvzero );
	setShape( shape );
	return true;
}

void Plant::hit( float damage )
{
	Char.recieveDamage( damage );
}
