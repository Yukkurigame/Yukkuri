/*
 * Features.cpp
 *
 *  Created on: 13.04.2013
 */


#include "map/generator/modules/Features.h"
#include "debug.h"
#include "hacks.h"


namespace MapFeatures
{
	void assignBiomes( );


	MapProto* proto = NULL;
	GeneratorModule module = {	&init, &clean, &process	};
}



void MapFeatures::init( )
{
	proto = NULL;
}


void MapFeatures::process( MapProto* pr )
{
	proto = pr;

	Debug::debug( Debug::MAP, "Decorate map...\n" );
	assignBiomes();
}


void MapFeatures::clean( )
{
	proto = NULL;
}



GeneratorModule* MapFeatures::get_module( )
{
	return &module;
}


void MapFeatures::assignBiomes( )
{
	Center* p;
	FOREACH1( p, proto->centers ) {
		p->biome = MapGenerator::getBiome( p );
	}
}

