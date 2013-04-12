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
#include "delaunay/geom/Point.h"
#include "delaunay/delaunay/Voronoi.h"
#include "map/generator/generator_constants.h"
#include "map/generator/IslandShape.h"
#include "3rdparty/rand31.h"
#include "basic_types.h"


namespace MapGenerator
{
	// These store the graph data
	std::vector< Center* > centers;
	std::vector< Corner* > corners;
	std::vector< Edge* > edges;

	MapGenerator( float size );
	virtual ~MapGenerator( );

	// Random parameters governing the overall shape of the island
	void newIsland( IslandForm type, const char* seed_string );

	void reset( );

	void go( int first, int last );

	// Look up a Voronoi Edge object given two adjacent Voronoi
	// polygons, or two adjacent Voronoi corners
	Edge* lookupEdgeFromCenter( Center* p, Center* r );

	Edge* lookupEdgeFromCorner( Corner* q, Corner* s );

	void assignBiomes( );

	// Dump all generated stuff to engine-readable structure.
	void dumpMap( const char* path );

	// Island details are controlled by this random generator. The
	// initial map upon loading is always deterministic, but
	// subsequent maps reset this random number generator with a
	// random seed.
	rand31 mapRandom;

}

private:
	/* Generate random points and assign them to be on the island or
	 * in the water. Some water points are inland lakes; others are
	 * ocean. We'll determine ocean later by looking at what's
	 * connected to ocean.
	 */
	void generateRandomPoints( std::vector< Delaunay::Point* >& pts );

	/* Improve the random set of points with Lloyd Relaxation. */
	void improveRandomPoints( std::vector< Delaunay::Point* >& pts );

	/* Although Lloyd relaxation improves the uniformity of polygon
	 * sizes, it doesn't help with the edge lengths. Short edges can
	 * be bad for some games, and lead to weird artifacts on
	 * rivers. We can easily lengthen short edges by moving the
	 * corners, but **we lose the Voronoi property**.  The corners are
	 * moved to the average of the polygon centers around them. Short
	 * edges become longer. Long edges tend to become shorter. The
	 * polygons tend to be more uniform after this step.
	 */
	void improveCorners( );

	/* Create an array of corners that are on land only, for use by
	 * algorithms that work only on land.  We return an array instead
	 * of a vector because the redistribution algorithms want to sort
	 * this array using Array.sortOn.
	 */
	list< Corner* > landCorners( const std::vector< Corner* >& c );

	/* Build graph data structure in 'edges', 'centers', 'corners',
	 * based on information in the Voronoi results: point.neighbors
	 * will be a list of neighboring points of the same type (corner
	 * or center); point.edges will be a list of edges that include
	 * that point. Each edge connects to four points: the Voronoi edge
	 * edge.{v0,v1} and its dual Delaunay triangle edge edge.{d0,d1}.
	 * For boundary polygons, the Delaunay edge will have one null
	 * point, and the Voronoi edge may be null.
	 */
	void buildGraph( const std::vector< Delaunay::Point* >& pts, Delaunay::Voronoi* voronoi );

	void generateHeightMap( );
	void assignElevation( );

	/* Determine elevations and water at Voronoi corners. By
	 * construction, we have no local minima. This is important for
	 * the downslope vectors later, which are used in the river
	 * construction algorithm. Also by construction, inlets/bays
	 * push low elevation areas inland, which means many rivers end
	 * up flowing out through them. Also by construction, lakes
	 * often end up on river paths because they don't raise the
	 * elevation as much as other terrain does.
	 */
	void assignCornerElevations( );

	// Change the overall distribution of elevations so that lower
	// elevations are more common than higher
	// elevations. Specifically, we want elevation X to have frequency
	// (1-X).  To do this we will sort the corners, then set each
	// corner to its desired elevation.
	void redistributeElevations( list< Corner* >& );

	void assignTemperature( );
	void redistributeTemperature( );


	// Change the overall distribution of moisture to be evenly distributed.
	void redistributeMoisture( list< Corner* >& );

	// Determine polygon and corner types: ocean, coast, land.
	void assignOceanCoastAndLand( );

	// Polygon elevations are the average of the elevations of their corners.
	void assignPolygonElevations( );

	// Calculate downslope pointers.  At every point, we point to the
	// point downstream from it, or to itself.  This is used for
	// generating rivers and watersheds.
	void calculateDownslopes( );

	// Calculate the watershed of every land point. The watershed is
	// the last downstream land point in the downslope graph. TODO:
	// watersheds are currently calculated on corners, but it'd be
	// more useful to compute them on polygon centers so that every
	// polygon can be marked as being in one watershed.
	void calculateWatersheds( );

	// Create rivers along edges. Pick a random corner point, then
	// move downslope. Mark the edges and corners as rivers.
	void createRivers( );

	// Calculate moisture. Freshwater sources spread moisture: rivers
	// and lakes (not oceans). Saltwater sources have moisture but do
	// not spread it (we set it at the end, after propagation).
	void assignCornerMoisture( );

	// Polygon moisture is the average of the moisture at corners
	void assignPolygonMoisture( );

	// Assign a biome type to each polygon. If it has
	// ocean/coast/water, then that's the biome; otherwise it depends
	// on low/high elevation and low/medium/high moisture. This is
	// roughly based on the Whittaker diagram but adapted to fit the
	// needs of the island map generator.
	static Biome getBiome( Center * p );

	// Determine whether a given point should be on the island or in the water.
	bool inside( const s2f* p );

	///////////////////////////////////////////
	// VARIABLES

	static const int NUM_POINTS = 2000;
	static const float LAKE_THRESHOLD = 0.3; // 0 to 1, fraction of water corners for water polygon
	static const int NUM_LLOYD_ITERATIONS = 4;
	static const int min_size = 32;


	// Passed in by the caller:
	int initial_seed;
	char* initial_seed_string;
	float SIZE;

	// Geographical position
	float latitude;
	float longitude;

	// Island shape is controlled by the islandRandom seed and the
	// type of island, passed in when we set the island shape. The
	// islandShape function uses both of them to determine whether any
	// point should be water or land.
	ShapeGenerateFunc* islandShape;

	HeightMap heights;

	// These store the graph data
	std::vector< Delaunay::Point* > points;  // Only useful during map construction
};

#endif /* GENERATORMAP_H_ */
