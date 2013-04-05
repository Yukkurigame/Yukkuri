/*
 * bson.cpp
 *
 *  Created on: 05.04.2013
 */

#include "utils/bson.h"
#include "map/Tiles.h"

namespace BSON
{

template<>
int pack( char*& output, int& length, ChunkTile* data )
{

	return -1;
}


template<>
int pack( char*& output, int& length, RegionMap* data )
{

	return -1;
}

}
