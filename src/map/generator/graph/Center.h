/*
 * Center.h
 *
 *  Created on: 02.03.2013
 */

#ifndef CENTER_H_
#define CENTER_H_

#include "map/generator/graph/Edge.h"
#include "map/generator/graph/Corner.h"
#include "map/generator/Constants.h"
#include "utils/list.h"
#include "basic_types.h"



class Center {
public:
	Center( );
	virtual ~Center( );

	int index;
	s2f point;  // location
	bool water;  // lake or ocean
	bool ocean;  // ocean
	bool coast;  // land polygon touching an ocean
	bool border;  // at the edge of the map
	Biome biome;  // biome type (see article)
	float elevation;  // 0.0-1.0
	float moisture;  // 0.0-1.0
	float temperature; // Node temperature

	list<Center*> neighbors;
	list<Edge*> borders;
	list< Corner* > corners;

	inline float property( NodeProperty p ){
		switch( p ){
			case gnpElevation:
				return elevation;
				break;
			case gnpMoisture:
				return moisture;
				break;
			case gnpTemperature:
				return temperature;
				break;
		}
		return 0.0;
	}
};

#endif /* CENTER_H_ */
