/*
 * MapGen.h
 *
 *  Created on: 03.03.2013
 */

#ifndef MAPRENDER_H_
#define MAPRENDER_H_

#include "map/generator/MapGenerator.h"


namespace MapRender
{
	void init();
	void clean();
	void process( MapProto* );

	GeneratorModule* get_module();
};

#endif /* MAPRENDER_H_ */
