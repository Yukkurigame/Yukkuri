/*
 * GeneratorMap.h
 *
 *  Created on: 02.03.2013
 */

#ifndef GENERATORMAP_H_
#define GENERATORMAP_H_

#include <vector>

#include "graph/Center.h"
#include "graph/Edge.h"
#include "graph/Corner.h"
#include "map/generator/Constants.h"
#include "map/generator/IslandShape.h"
#include "graphics/GraphicsTypes.h"
#include "3rdparty/rand31.h"
#include "basic_types.h"


struct MapProto
{
	// These store the graph data
	std::vector< Center* > centers;
	std::vector< Corner* > corners;
	std::vector< Edge* > edges;

	// Passed in by the caller:
	int initial_seed;
	char* initial_seed_string;
	IslandForm type;

	// Image
	Texture atlas;
	UINT texture_id;

	// Geographical position
	float latitude;
	float longitude;

	// Island details are controlled by this random generator. The
	// initial map upon loading is always deterministic, but
	// subsequent maps reset this random number generator with a
	// random seed.
	rand31 mapRandom;
};


struct GeneratorModule
{
	void (*init)();
	void (*clean)();
	void (*process)(MapProto*);
};


namespace MapGenerator
{
	void init( );
	void clean( );

	// Random parameters governing the overall shape of the island
	void newIsland( IslandForm type, const char* seed_string );
	void go( );

	MapProto* get_proto();

	list< Corner* > landCorners( MapProto* p );

	// Look up a Voronoi Edge object given two adjacent Voronoi
	// polygons, or two adjacent Voronoi corners
	Edge* lookupEdgeFromCenter( Center* p, Center* r );
	Edge* lookupEdgeFromCorner( Corner* q, Corner* s );

	Biome getBiome( Center * p );

	// Dump all generated stuff to engine-readable structure.
	void dumpMap( const char* path );

};


#endif /* GENERATORMAP_H_ */
