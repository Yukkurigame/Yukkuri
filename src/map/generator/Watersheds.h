/*
 * Watersheds.h
 *
 *  Created on: 02.03.2013
 */

#ifndef WATERSHEDS_H_
#define WATERSHEDS_H_

#include "GeneratorMap.h"
#include "graph/Center.h"
#include "graph/Corner.h"
#include "basic_types.h"


class Watersheds {
public:
	Watersheds( );
	virtual ~Watersheds( );

	std::map< int, int > lowestCorner;  // polygon index -> corner index
	std::map< int, int > watersheds;  // polygon index -> corner index

	// We want to mark each polygon with the corner where water would
	// exit the island.
	void createWatersheds( MapGenerator& map );

};

#endif /* WATERSHEDS_H_ */
