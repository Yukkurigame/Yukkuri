/*
 * GeneratorMap.cpp
 *
 *  Created on: 02.03.2013
 */

#include "map/generator/MapGenerator.h"
#include "map/generator/modules/Shape.h"
#include "map/generator/modules/Temperature.h"
#include "map/generator/modules/Water.h"
#include "map/generator/modules/Features.h"
#include "map/generator/modules/Render.h"

#include "utils/list.h"
#include "debug.h"
#include "safestring.h"
#include "hacks.h"



namespace MapGenerator
{
	list< GeneratorModule* > modules;
	MapProto* proto = NULL;
}


void MapGenerator::init( )
{
	if( !modules.count ){
		modules.push_back( MapShape::get_module() );
		modules.push_back( MapTemperature::get_module() );
		modules.push_back( MapWater::get_module() );
		modules.push_back( MapFeatures::get_module() );
		modules.push_back( MapRender::get_module() );
	}

	ITER_LIST( GeneratorModule*, modules )
		it->data->init();
}


void MapGenerator::clean( )
{
	ITER_LIST( GeneratorModule*, modules )
			it->data->clean();
	modules.clear();
}


MapProto* MapGenerator::get_proto()
{
	return proto;
}


void MapGenerator::newIsland( IslandForm type, const char* seed_string )
{
	int seed = 412496234;

	if( proto )
		delete proto;

	proto = new MapProto();
	Debug::debug( Debug::MAP, "Shaping map...\n" );

	if( seed_string && seed_string[0] != '\0' ){
		UINT len = strlen( seed_string );
		// Convert the string into a number. This is a cheesy way to
		// do it but it doesn't matter. It just allows people to use
		// words as seeds.
		for( UINT i = 0; i < len; ++i ){
			seed = ( seed << 4 ) | (UINT)seed_string[i];
		}
		seed %= 100000;
	}

	proto->initial_seed_string = strdup( seed_string );
	proto->initial_seed = seed;
	proto->mapRandom.seed( seed );
	proto->latitude = proto->mapRandom.nextDouble( -90.0, 90.0 );
	proto->longitude = proto->mapRandom.nextDouble( -180.0, 180.0 );

}


void MapGenerator::go( )
{
	//roads = new Roads();
	//lava = new Lava();
	//roads.createRoads(map);
	//lava.createLava(map, map.mapRandom.nextDouble);
	//noisyEdges.buildNoisyEdges( map->centers, /*lava,*/ &map->mapRandom );

	ITER_LIST( GeneratorModule*, modules )
		it->data->process( proto );

	dumpMap(".");
}


/**
 * Create an array of corners that are on land only, for use by
 * algorithms that work only on land.  We return an array instead
 * of a vector because the redistribution algorithms want to sort
 * this array using Array.sortOn.
 */
list< Corner* > MapGenerator::landCorners( MapProto* p )
{
	list< Corner* > locations;
	Corner* q = NULL;
	FOREACH1( q, p->corners ) {
		if( !q->ocean && !q->coast )
			locations.push_back( q );
	}
	return locations;
}


Edge* MapGenerator::lookupEdgeFromCenter( Center* p, Center* r )
{
	ITER_LIST( Edge*, p->borders ){
		Edge* edge = it->data;
		if( edge->d0 == r || edge->d1 == r )
			return edge;
	}
	return NULL;
}

Edge* MapGenerator::lookupEdgeFromCorner( Corner* q, Corner* s )
{
	ITER_LIST( Edge*, q->protrudes ){
		Edge* edge = it->data;
		if( edge->v0 == s || edge->v1 == s )
			return edge;
	}
	return NULL;
}


/**
 * Assign a biome type to each polygon. If it has
 * ocean/coast/water, then that's the biome; otherwise it depends
 * on low/high elevation and low/medium/high moisture. This is
 * roughly based on the Whittaker diagram but adapted to fit the
 * needs of the island map generator.
 */
Biome MapGenerator::getBiome( Center * p )
{
	if( p->ocean ){
		return bOCEAN;
	}else if( p->water ){
		if( p->elevation < 0.1 )
			return bMARSH;
		if( p->elevation > 0.8 )
			return bICE;
		return bLAKE;
	}else if( p->coast ){
		return bBEACH;
	}else if( p->temperature < 0.0 ){
		if( p->moisture > 0.50 )
			return bSNOW;
		else if( p->moisture > 0.33 )
			return bTUNDRA;
		else if( p->moisture > 0.16 )
			return bBARE;
		else
			return bSCORCHED;
	}else if( p->temperature < 5.0 ){
		if( p->moisture > 0.66 )
			return bTAIGA;
		else if( p->moisture > 0.33 )
			return bSHRUBLAND;
		else
			return bTEMPERATE_DESERT;
	}else if( p->temperature < 25.0 ){
		if( p->moisture > 0.83 )
			return bTEMPERATE_RAIN_FOREST;
		else if( p->moisture > 0.50 )
			return bTEMPERATE_DECIDUOUS_FOREST;
		else if( p->moisture > 0.16 )
			return bGRASSLAND;
		else
			return bTEMPERATE_DESERT;
	}else{
		if( p->moisture > 0.66 )
			return bTROPICAL_RAIN_FOREST;
		else if( p->moisture > 0.33 )
			return bTROPICAL_SEASONAL_FOREST;
		else if( p->moisture > 0.16 )
			return bGRASSLAND;
		else
			return bSUBTROPICAL_DESERT;
	}
}
