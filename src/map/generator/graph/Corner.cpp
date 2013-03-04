/*
 * Corner.cpp
 *
 *  Created on: 02.03.2013
 */

#include "Corner.h"

Corner::Corner( ) : index(), point(), elevation(), moisture(),
	river(), downslope(NULL), watershed(NULL), watershed_size()
{

}

Corner::~Corner( )
{
	touches.clear();
	protrudes.clear();
	adjacent.clear();
}

