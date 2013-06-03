/*
 * Water.h
 *
 *  Created on: 13.04.2013
 */

#ifndef GENERATOR_WATER_H_
#define GENERATOR_WATER_H_

#include "map/generator/MapGenerator.h"


namespace MapWater
{

	void init( );
	void process( MapProto* );
	void clean( );

	GeneratorModule* get_module();
}


#endif /* GENERATOR_WATER_H_ */
