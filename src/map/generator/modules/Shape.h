/*
 * Shape.h
 *
 *  Created on: 12.04.2013
 */

#ifndef GENERATOR_SHAPE_H_
#define GENERATOR_SHAPE_H_

#include "map/generator/MapGenerator.h"

namespace MapShape
{

	void init( );
	void process( MapProto* );
	void clean( );

	GeneratorModule* get_module();
}


#endif /* GENERATOR_SHAPE_H_ */
