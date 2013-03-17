/*
 * Watersheds.h
 *
 *  Created on: 02.03.2013
 */

#ifndef WATERSHEDS_H_
#define WATERSHEDS_H_

#include "map/generator/graph/Center.h"
#include "map/generator/graph/Corner.h"
#include "basic_types.h"
#include <map>
#include <vector>


class Watersheds {
public:
	Watersheds( );
	virtual ~Watersheds( );

	std::map< int, int > lowestCorner;  // polygon index -> corner index
	std::map< int, int > watersheds;  // polygon index -> corner index

	// We want to mark each polygon with the corner where water would
	// exit the island.
	void createWatersheds( std::vector< Center* >& centers );

};

#endif /* WATERSHEDS_H_ */
