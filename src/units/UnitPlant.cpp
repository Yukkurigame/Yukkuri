/*
 * Plant.cpp
 *
 *  Created on: 18.07.2010
 */
#include "UnitPlant.h"


void Plant::hit( float damage )
{
	Char.recieveDamage( damage );
}
