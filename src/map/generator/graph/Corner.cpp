/*
 * Corner.cpp
 *
 *  Created on: 02.03.2013
 */

#include "map/generator/graph/Corner.h"

Corner::Corner( ) : index(), point(), elevation(), moisture(),
	river(), downslope(0), watershed(0), watershed_size()
{

}

Corner::~Corner( )
{
	touches.clear();
	protrudes.clear();
	adjacent.clear();
}

