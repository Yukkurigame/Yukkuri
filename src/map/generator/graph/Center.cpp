/*
 * Center.cpp
 *
 *  Created on: 02.03.2013
 */

#include "Center.h"

Center::Center( ) : index(), point(), water(false), ocean(false), coast(false),
	border(), biome(bNONE), elevation(), moisture()
{
}

Center::~Center( )
{
	neighbors.clear();
	borders.clear();
	corners.clear();
}

