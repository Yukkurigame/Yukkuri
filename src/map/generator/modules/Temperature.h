/*
 * Temperature.h
 *
 *  Created on: 13.04.2013
 */

#ifndef GENERATOR_TEMPERATURE_H_
#define GENERATOR_TEMPERATURE_H_

#include "map/generator/MapGenerator.h"


namespace MapTemperature
{

	void init( );
	void process( MapProto* );
	void clean( );

	GeneratorModule* get_module();
}



#endif /* GENERATOR_TEMPERATURE_H_ */
