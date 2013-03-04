/*
 * Corner.h
 *
 *  Created on: 02.03.2013
 */

#ifndef CORNER_H_
#define CORNER_H_

#include <vector>
#include "basic_types.h"

class Center;
class Edge;


class Corner {
public:
	Corner( );
	virtual ~Corner( );

	int index;
	s2f point;  // location
	bool ocean;  // ocean
	bool water;  // lake or ocean
	bool coast;  // touches ocean and land polygons
	bool border;  // at the edge of the map
	float elevation;  // 0.0-1.0
	float moisture;  // 0.0-1.0

	std::vector< Center* > touches;
	std::vector< Edge* > protrudes;
	std::vector< Corner* > adjacent;

	int river;  // 0 if no river, or volume of water in river
	Corner* downslope;  // pointer to adjacent corner most downhill
	Corner* watershed;  // pointer to coastal corner, or null
	int watershed_size;
};

#endif /* CORNER_H_ */
