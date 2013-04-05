/*
 * Corner.cpp
 *
 *  Created on: 02.03.2013
 */

#include "map/generator/graph/Corner.h"

Corner::Corner( ) : index(), point(), elevation(), moisture(),
	river(0), downslope(0), watershed(0), watershed_size(0)
{

}

Corner::~Corner( )
{
	touches.clear();
	protrudes.clear();
	adjacent.clear();
}

