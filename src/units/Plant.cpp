/*
 * Plant.cpp
 *
 *  Created on: 18.07.2010
 */
#include "Plant.h"


Plant::Plant( )
{

}

void Plant::hit( float damage )
{
	Char.recieveDamage( damage );
}
