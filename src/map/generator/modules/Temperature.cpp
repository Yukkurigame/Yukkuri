/*
 * Temperature.cpp
 *
 *  Created on: 13.04.2013
 */

#include "map/generator/modules/Temperature.h"
#include "debug.h"
#include "hacks.h"
#include <stdlib.h>

namespace MapTemperature
{
	void assignTemperature( );
	void redistributeTemperature( );

	MapProto* proto;
	GeneratorModule module = {	&init, &clean, &process	};
}


void MapTemperature::init( )
{
	proto = NULL;
}

void MapTemperature::process( MapProto* p )
{
	proto = p;

	Debug::debug( Debug::MAP, "Calculate temperature...\n" );
	assignTemperature();
	redistributeTemperature();
}

void MapTemperature::clean( )
{
	proto = NULL;
}


GeneratorModule* MapTemperature::get_module()
{
	return &module;
}


/**
 * Calculate temperature map for generated region
 * Rules of model:
 * All nodes get default region temperature.
 * - Default region temperature is based on geographical position.
 * - Each degree of latitude lowers temperature by 0.75 degree in direction from equator to pole.
 * - Biomes calculation must take into account annual temperature variations.
 * Individual nodes temperature.
 * - Depends on height. Each unit of height lowers temperature by 3 degree
 * - Depends of ocean position. Each border adjacent with ocean gives +5 degree to node,
 *   second-neighborhood gives +2 degree.
 * - Ocean influence must depend on region temperature. It must lower temperature for hot regions.
 * - Flatness of nodes. Bunch of nodes with slightly difference in height must receive
 *   overall delta temperature bonus.
 */
void MapTemperature::assignTemperature( )
{
	float temperature = 35.0 - abs(proto->latitude) * 0.75;

	Corner* q;
	FOREACH1( q, proto->corners ) {
		// Minimum temperature: 35 - 0.75 * 90 - 40 * 0.7 = -60.5 degree.
		q->temperature = temperature - ( q->elevation - 0.3 ) * 40;

		// Ocean normalizes temperature to some limit
		if( q->ocean ){
			if( q->temperature < 5 )
				q->temperature += 20;
			continue;
		}
	}

	// Assign calculated temperature to centers
	Center* p;
	FOREACH1( p, proto->centers ) {
		float temperature = 0.0;
		float corners = p->corners.count;
		ITER_LIST(Corner*, p->corners ) {
			temperature += it->data->temperature;
		}
		p->temperature = temperature / corners;
	}
}


/**
 * Make temperature field more smooth
 */
void MapTemperature::redistributeTemperature( )
{
/*

	// Coasts have ocean influence
	float first_neighbor = 0;
	float second_neighbor = 0;
	ITER_LIST( Center*, q->touches ){
		Center* c = it->data;
		if( c->ocean ){
			first_neighbor += 5;
			continue;
		}

		// search for second neighbor
		ITER_LIST2( Center*, c->neighbors ){
			Center* n = it->data;
			if( n->ocean )
				second_neighbor += 2;
		}
	}

	float first_temp = q->temperature + first_neighbor;
	q->temperature = ( first_temp > 35 ? 35 : first_temp );

	// Second neighborhood may give more heat.
	q->temperature += second_neighbor;
*/
}
