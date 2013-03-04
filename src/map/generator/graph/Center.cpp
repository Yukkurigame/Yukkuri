/*
 * Center.cpp
 *
 *  Created on: 02.03.2013
 */

#include "Center.h"

Center::Center( ) : index(), elevation(), moisture()
{
}

Center::~Center( )
{
	neighbors.clear();
	borders.clear();
	corners.clear();
}

