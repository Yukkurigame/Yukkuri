/*
 * Region.h
 *
 *  Created on: 27.05.2012
 *
 */

#ifndef REGION_H_
#define REGION_H_

#include "map/Tiles.h"


namespace Region
{
	bool init( );
	void clean( );
	void generate( UINT form, const char* seed );
	void load( const char* name );
	TileInfo* getTile( signed int x, signed int y );

}


#endif /* REGION_H_ */
