/*
 * Features.h
 *
 *  Created on: 13.04.2013
 */

#ifndef GENERATOR_FEATURES_H_
#define GENERATOR_FEATURES_H_

#include "map/generator/MapGenerator.h"


namespace MapFeatures
{
	void init( );
	void process( MapProto* );
	void clean( );

	GeneratorModule* get_module();
}



#endif /* GENERATOR_FEATURES_H_ */
