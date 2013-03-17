/*
 * GeneratorMap.cpp
 *
 *  Created on: 02.03.2013
 */

#include "map/generator/MapGenerator.h"
#include "delaunay/geom/Rectangle.h"
#include "delaunay/delaunay/Edge.h"
#include "utils/list.h"
#include "utils/misc.h"
#include "debug.h"
#include "hacks.h"
#include <map>
#include <algorithm>
#include <stdio.h>
#include <cmath>



MapGenerator::MapGenerator( float size )
{
	SIZE = size;
	reset();
}

MapGenerator::~MapGenerator( )
{
	reset();
}

void MapGenerator::newIsland( IslandForm type, int seed, int variant )
{
	switch( type ){
		case ifRadial:
			islandShape = IslandShape::makeRadial( seed );
			break;
		case ifPerlin:
			islandShape = IslandShape::makePerlin( seed );
			break;
		case ifSquare:
			islandShape = IslandShape::makeSquare( seed );
			break;
		case ifBlob:
			islandShape = IslandShape::makeBlob( seed );
			break;
		default:
			break;
	}
	mapRandom.seed = variant;
}

void MapGenerator::reset( )
{
	// Break cycles so the garbage collector will release data.
	points.clear();
	clear_vector( &edges );
	clear_vector( &centers );
	clear_vector( &corners );
}

void MapGenerator::go( int first, int last )
{

#define CHECK_LAST( num )	\
	if( last <= num )		\
		break;				\

	// Lol
	switch( first ){
		case 0:
		{
			// Generate the initial random set of points
			Debug::debug( Debug::MAP, "Place points...\n" );
			reset();
			generateRandomPoints( points );
			CHECK_LAST( 1 )
		}
		case 1:
		{
			Debug::debug( Debug::MAP, "Improve points...\n" );
			improveRandomPoints( points );
			CHECK_LAST( 2 )
		}
		case 2:
		{
			// Create a graph structure from the Voronoi edge list. The
			// methods in the Voronoi object are somewhat inconvenient for
			// my needs, so I transform that data into the data I actually
			// need: edges connected to the Delaunay triangles and the
			// Voronoi polygons, a reverse map from those four points back
			// to the edge, a map from these four points to the points
			// they connect to (both along the edge and crosswise).
			Debug::debug( Debug::MAP, "Build graph...\n" );
			Delaunay::Rectangle r( 0, 0, SIZE, SIZE );
			Delaunay::Voronoi* voronoi = new Delaunay::Voronoi( points, NULL, r );
			buildGraph( points, voronoi );
			improveCorners();
			delete voronoi;
			clear_vector( &points );
			CHECK_LAST( 3 )
		}
		case 3:
		{
			Debug::debug( Debug::MAP, "Assign elevations...\n" );
			// Determine the elevations and water at Voronoi corners.
			assignCornerElevations();

			// Determine polygon and corner type: ocean, coast, land.
			assignOceanCoastAndLand();

			// Rescale elevations so that the highest is 1.0, and they're
			// distributed well. We want lower elevations to be more common
			// than higher elevations, in proportions approximately matching
			// concentric rings. That is, the lowest elevation is the
			// largest ring around the island, and therefore should more
			// land area than the highest elevation, which is the very
			// center of a perfectly circular island.
			std::vector< Corner* > cnrs = landCorners( corners );
			redistributeElevations( cnrs );

			// Assign elevations to non-land corners
			Corner* q;
			FOREACH1( q, corners ) {
				if( q->ocean || q->coast )
					q->elevation = 0.0;
			}

			// Polygon elevations are the average of their corners
			assignPolygonElevations();
			CHECK_LAST( 4 )
		}
		case 4:
		{
			Debug::debug( Debug::MAP, "Assign moisture...\n" );
			// Determine downslope paths.
			calculateDownslopes();

			// Determine watersheds: for every corner, where does it flow
			// out into the ocean?
			calculateWatersheds();

			// Create rivers.
			createRivers();

			// Determine moisture at corners, starting at rivers
			// and lakes, but not oceans. Then redistribute
			// moisture to cover the entire range evenly from 0.0
			// to 1.0. Then assign polygon moisture as the average
			// of the corner moisture.
			assignCornerMoisture();
			std::vector< Corner* > cnrs = landCorners( corners );
			redistributeMoisture( cnrs );
			assignPolygonMoisture();
			CHECK_LAST( 5 )
		}
		case 5:
		{
			Debug::debug( Debug::MAP, "Decorate map...\n" );
			assignBiomes();
			CHECK_LAST( 6 )
		}
		default:
			break;
	}

}

void MapGenerator::generateRandomPoints( std::vector< Delaunay::Point* >& pts )
{

	for( int i = 0; i < NUM_POINTS; i++ ){
		Delaunay::Point* p = new Delaunay::Point(
				mapRandom.nextDoubleRange( 10, SIZE - 10 ),
				mapRandom.nextDoubleRange( 10, SIZE - 10 ) );
		pts.push_back( p );
	}
	//printPoints("vt");
}

void MapGenerator::improveRandomPoints( std::vector< Delaunay::Point* >& pts )
{
	// We'd really like to generate "blue noise". Algorithms:
	// 1. Poisson dart throwing: check each new point against all
	//     existing points, and reject it if it's too close.
	// 2. Start with a hexagonal grid and randomly perturb points.
	// 3. Lloyd Relaxation: move each point to the centroid of the
	//     generated Voronoi polygon, then generate Voronoi again.
	// 4. Use force-based layout algorithms to push points away.
	// 5. More at http://www.cs.virginia.edu/~gfx/pubs/antimony/
	// Option 3 is implemented here. If it's run for too many iterations,
	// it will turn into a grid, but convergence is very slow, and we only
	// run it a few times.

	Delaunay::Rectangle size( 0, 0, SIZE, SIZE );
	for( int i = 0; i < NUM_LLOYD_ITERATIONS; i++ ){
		Delaunay::Voronoi* voronoi = new Delaunay::Voronoi( points, NULL, size );
		Delaunay::Point* p = NULL;
		FOREACH1( p, points ) {
			std::vector< Delaunay::Point* > region = voronoi->region( p );
			p->x = 0.0;
			p->y = 0.0;
			Delaunay::Point* q = NULL;
			FOREACH2( q, region ) {
				p->x += q->x;
				p->y += q->y;
			}
			int region_size = region.size();
			p->x /= region_size;
			p->y /= region_size;
		}
		delete voronoi;
	}

	//printPoints("ivt");
}


// Although Lloyd relaxation improves the uniformity of polygon
// sizes, it doesn't help with the edge lengths. Short edges can
// be bad for some games, and lead to weird artifacts on
// rivers. We can easily lengthen short edges by moving the
// corners, but **we lose the Voronoi property**.  The corners are
// moved to the average of the polygon centers around them. Short
// edges become longer. Long edges tend to become shorter. The
// polygons tend to be more uniform after this step.
void MapGenerator::improveCorners( )
{
	std::map< int, s2f > newCorners;

	// First we compute the average of the centers next to each corner.
	Corner* q = NULL;
	FOREACH1( q, corners ) {
		if( q->border ){
			newCorners[q->index] = q->point;
		}else{
			s2f point;
			int size = q->touches.count;
			ITER_LIST( Center*, q->touches ) {
				Center* r = it->data;
				point.x += r->point.x;
				point.y += r->point.y;
			}
			point.x /= size;
			point.y /= size;
			newCorners[q->index] = point;
		}
	}

	// Move the corners to the new locations.
	for( UINT i = 0; i < corners.size(); i++ ){
		corners[i]->point = newCorners[i];
	}


	// The edge midpoints were computed for the old corners and need
	// to be recomputed.
	Edge* edge;
	FOREACH1( edge, edges ) {
		if( edge->v0 && edge->v1 )
			edge->midpoint = interpolate( edge->v0->point, edge->v1->point, 0.5 );
	}
	printPoints("t");
}

std::vector< Corner* > MapGenerator::landCorners( const std::vector< Corner* >& c )
{
	std::vector< Corner* > locations;
	Corner* q = NULL;
	FOREACH1( q, c ) {
		if( !q->ocean && !q->coast )
			locations.push_back( q );
	}
	return locations;
}

Corner* makeCorner( std::vector< Corner* >& corners,
		std::map< int, std::vector< Corner* > >& _cornerMap, const Delaunay::Point* point,
		int SIZE )
{
	if( point == NULL )
		return NULL;
	for( int bucket = (int)point->x - 1; bucket <= (int)point->x + 1; ++bucket ){
		Corner* q = NULL;
		FOREACH1( q, _cornerMap[bucket] ) {
			double dx = point->x - q->point.x;
			double dy = point->y - q->point.y;
			if( dx * dx + dy * dy < 1e-6 )
				return q;
		}
	}

	int bucket = (int)point->x;
	Corner* q = new Corner();
	q->index = corners.size();
	corners.push_back( q );
	q->point.x = point->x;
	q->point.y = point->y;
	q->border = ( point->x == 0 || point->x == SIZE || point->y == 0 || point->y == SIZE );
	_cornerMap[bucket].push_back( q );
	return q;
}

// M-M-MEGASLOW
void MapGenerator::buildGraph( const std::vector< Delaunay::Point* >& pts,
		Delaunay::Voronoi* voronoi )
{
	std::map< float, std::map< float, Center* > > centerLookup;

	// Build Center objects for each of the points, and a lookup map
	// to find those Center objects again as we build the graph
	Delaunay::Point* point;
	FOREACH1( point, pts ){
		Center* p = new Center();
		p->index = centers.size();
		p->point = s2f(point->x, point->y);
		centers.push_back(p);
		centerLookup[point->x][point->y] = p;
	}

	// We need to prepare regions before
	// Edges or neighboringSites are available
	voronoi->regionsPrepare();

	// The Voronoi library generates multiple Point objects for
	// corners, and we need to canonicalize to one Corner object.
	// To make lookup fast, we keep an array of Points, bucketed by
	// x value, and then we only have to look at other Points in
	// nearby buckets. When we fail to find one, we'll create a new
	// Corner object.
	std::map< int, std::vector< Corner* > > _cornerMap;

	const std::vector< Delaunay::Edge* >& libedges = voronoi->edges();
	Delaunay::Edge* libedge;
	FOREACH1( libedge, libedges ) {
		Delaunay::LineSegment* dedge = libedge->delaunayLine();
		Delaunay::LineSegment* vedge = libedge->voronoiEdge();

		// Fill the graph data. Make an Edge object corresponding to
		// the edge from the voronoi library.
		Edge* edge = new Edge;

		edge->index = edges.size();
		edge->river = 0;
		edges.push_back( edge );
		if( vedge->p0 && vedge->p1 ){
			s2f vp0( vedge->p0->x, vedge->p0->y );
			s2f vp1( vedge->p1->x, vedge->p1->y );
			edge->midpoint = interpolate( vp0, vp1, 0.5 );
		}

		// Edges point to corners. Edges point to centers.
		edge->v0 = makeCorner( corners, _cornerMap, vedge->p0, SIZE );
		edge->v1 = makeCorner( corners, _cornerMap, vedge->p1, SIZE );
		edge->d0 = centerLookup[dedge->p0->x][dedge->p0->y];
		edge->d1 = centerLookup[dedge->p1->x][dedge->p1->y];

		// Centers point to edges. Corners point to edges.
		if( edge->d0 )
			edge->d0->borders.push_back( edge );
		if( edge->d1 )
			edge->d1->borders.push_back( edge );
		if( edge->v0 )
			edge->v0->protrudes.push_back( edge );
		if( edge->v1 )
			edge->v1->protrudes.push_back( edge );

#define ADD_TO_LIST( v, x )	\
	if( x && !v.find(x) )	\
		v.push_back(x);

		// Centers point to centers.
		if( edge->d0 && edge->d1 ){
			ADD_TO_LIST( edge->d0->neighbors, edge->d1 );
			ADD_TO_LIST( edge->d1->neighbors, edge->d0 );
		}

		// Corners point to corners
		if( edge->v0 && edge->v1 ){
			ADD_TO_LIST( edge->v0->adjacent, edge->v1 );
			ADD_TO_LIST( edge->v1->adjacent, edge->v0 );
		}

		// Centers point to corners
		if( edge->d0 ){
			ADD_TO_LIST( edge->d0->corners, edge->v0 );
			ADD_TO_LIST( edge->d0->corners, edge->v1 );
		}
		if( edge->d1 ){
			ADD_TO_LIST( edge->d1->corners, edge->v0 );
			ADD_TO_LIST( edge->d1->corners, edge->v1 );
		}

		// Corners point to centers
		if( edge->v0 ){
			ADD_TO_LIST( edge->v0->touches, edge->d0 );
			ADD_TO_LIST( edge->v0->touches, edge->d1 );
		}
		if( edge->v1 ){
			ADD_TO_LIST( edge->v1->touches, edge->d0 );
			ADD_TO_LIST( edge->v1->touches, edge->d1 );
		}
	}
}

void MapGenerator::assignCornerElevations( )
{
	list< Corner* > queue;

	// Make island?
	Corner* q;
	FOREACH1( q, corners ) {
		q->water = !inside( &q->point );
		// The edges of the map are elevation 0
		if( q->border ){
			q->elevation = 0.0;
			queue.push_back( q );
		}else{
			q->elevation = INFINITY;
		}
	}

	// Traverse the graph and assign elevations to each point. As we
	// move away from the map border, increase the elevations. This
	// guarantees that rivers always have a way down to the coast by
	// going downhill (no local minima).
	while( queue.count > 0 ){
		Corner* q = queue.pop();
		ITER_LIST( Corner*, q->adjacent ) {
			Corner* s = it->data;
			// Every step up is epsilon over water or 1 over land. The
			// number doesn't matter because we'll rescale the
			// elevations later.
			double newElevation = 0.01 + q->elevation;
			if( !q->water && !s->water )
				newElevation += 1;

			// If this point changed, we'll add it to the queue so
			// that we can process its neighbors too.
			if( newElevation < s->elevation ){
				s->elevation = newElevation;
				queue.push_back( s );
			}
		}
	}
}

bool compareElevations( const Corner* a, const Corner* b )
{
	return a->elevation < b->elevation;
}

void MapGenerator::redistributeElevations( std::vector< Corner* >& locations )
{
	// SCALE_FACTOR increases the mountain area. At 1.0 the maximum
	// elevation barely shows up on the map, so we set it to 1.1.
	const double SCALE_FACTOR = 1.1;

	std::sort( locations.begin(), locations.end(), compareElevations );
	int length = locations.size();
	for( int i = 0; i < length; i++ ){
		// Let y(x) be the total area that we want at elevation <= x.
		// We want the higher elevations to occur less than lower
		// ones, and set the area to be y(x) = 1 - (1-x)^2.
		double y = (double)i / (double)( length - 1 );
		// Now we have to solve for x, given the known y.
		//  *  y = 1 - (1-x)^2
		//  *  y = 1 - (1 - 2x + x^2)
		//  *  y = 2x - x^2
		//  *  x^2 - 2x + y = 0
		// From this we can use the quadratic equation to get:
		double x = sqrt( SCALE_FACTOR ) - sqrt( SCALE_FACTOR * ( 1.0 - y ) );
		if( x > 1.0 )
			x = 1.0;  // TODO: does this break downslopes?
		locations[i]->elevation = x;
	}
}

bool compareMoisture( const Corner* a, const Corner* b )
{
	return a->moisture < b->moisture;
}

void MapGenerator::redistributeMoisture( std::vector< Corner* >& locations )
{
	std::sort( locations.begin(), locations.end(), compareMoisture );
	int length = locations.size();
	for( int i = 0; i < length; i++ ){
		locations[i]->moisture = (double)i / (double)( length - 1 );
	}
}

void MapGenerator::assignOceanCoastAndLand( )
{
	// Compute polygon attributes 'ocean' and 'water' based on the
	// corner attributes. Count the water corners per
	// polygon. Oceans are all polygons connected to the edge of the
	// map. In the first pass, mark the edges of the map as ocean;
	// in the second pass, mark any water-containing polygon
	// connected an ocean as ocean.
	list< Center* > queue;
	Center* p;

	FOREACH1( p, centers ) {
		int numWater = 0;
		ITER_LIST(Corner*, p->corners ) {
			Corner* q = it->data;
			if( q->border ){
				p->border = true;
				p->ocean = true;
				q->water = true;
				queue.push_back( p );
			}
			if( q->water )
				numWater++;
		}
		p->water = ( p->ocean || numWater >= p->corners.count * LAKE_THRESHOLD );
	}
	while( queue.count ){
		Center* p = queue.pop();
		ITER_LIST( Center*, p->neighbors ){
			Center* r = it->data;
			if( r->water && !r->ocean ){
				r->ocean = true;
				queue.push_back( r );
			}
		}
	}

	// Set the polygon attribute 'coast' based on its neighbors. If
	// it has at least one ocean and at least one land neighbor,
	// then this is a coastal polygon.
	FOREACH1( p, centers ) {
		int numOcean = 0;
		int numLand = 0;
		ITER_LIST( Center*, p->neighbors ) {
			numOcean += (int)it->data->ocean;
			numLand += (int)!it->data->water;
		}
		p->coast = ( numOcean > 0 ) && ( numLand > 0 );
	}

	// Set the corner attributes based on the computed polygon
	// attributes. If all polygons connected to this corner are
	// ocean, then it's ocean; if all are land, then it's land;
	// otherwise it's coast.
	Corner* q;
	FOREACH1( q, corners ) {
		UINT numOcean = 0;
		UINT numLand = 0;
		ITER_LIST( Center*, q->touches ){
			p = it->data;
			numOcean += (int)p->ocean;
			numLand += (int)!p->water;
		}
		q->ocean = ( numOcean == q->touches.count );
		q->coast = ( numOcean > 0 ) && ( numLand > 0 );
		q->water = q->border || ( ( numLand != q->touches.count ) && !q->coast );
	}
}

void MapGenerator::assignPolygonElevations( )
{
	Center* p;
	FOREACH1( p, centers ) {
		double sumElevation = 0.0;
		ITER_LIST( Corner*, p->corners ) {
			sumElevation += it->data->elevation;
		}
		p->elevation = (double)sumElevation / (double)p->corners.count;
	}
}

void MapGenerator::calculateDownslopes( )
{
	Corner* q;
	FOREACH1( q, corners ) {
		Corner* r = q;
		ITER_LIST( Corner*, q->adjacent ){
			if( it->data->elevation <= r->elevation )
				r = it->data;
		}
		q->downslope = r;
	}
}

void MapGenerator::calculateWatersheds( )
{

	// Initially the watershed pointer points downslope one step.
	Corner* q;
	FOREACH1( q, corners ) {
		q->watershed = q;
		if( !q->ocean && !q->coast )
			q->watershed = q->downslope;
	}

	// Follow the downslope pointers to the coast. Limit to 100
	// iterations although most of the time with NUM_POINTS=2000 it
	// only takes 20 iterations because most points are not far from
	// a coast.  TODO: can run faster by looking at
	// p.watershed.watershed instead of p.downslope.watershed.
	for( int i = 0; i < 100; ++i ){
		bool changed = false;
		Corner* q;
		FOREACH1( q, corners ) {
			if( !q->ocean && !q->coast && !q->watershed->coast ){
				Corner* r = q->downslope->watershed;
				if( !r->ocean )
					q->watershed = r;
				changed = true;
			}
		}
		if( !changed )
			break;
	}

	// How big is each watershed?
	FOREACH1( q, corners ) {
		Corner* r = q->watershed;
		r->watershed_size = 1 + r->watershed_size;
	}
}

void MapGenerator::createRivers( )
{
	int length = corners.size();
	if( !length )
		return;

	for( int i = 0; i < SIZE / 2; i++ ){
		Corner* q = corners[mapRandom.nextIntRange( 0, length - 1 )];
		if( q->ocean || q->elevation < 0.3 || q->elevation > 0.9 )
			continue;

		// Bias rivers to go west: if (q.downslope.x > q.x) continue;
		while( !q->coast ){
			if( q == q->downslope )
				break;
			Edge* edge = lookupEdgeFromCorner( q, q->downslope );
			edge->river = edge->river + 1;
			q->river = ( q->river || 0 ) + 1;
			q->downslope->river = ( q->downslope->river || 0 ) + 1; // TODO: fix double count
			q = q->downslope;
		}
	}
}

void MapGenerator::assignCornerMoisture( )
{
	list< Corner* > queue;
	// Fresh water
	Corner* q;
	FOREACH1( q, corners ) {
		if( ( q->water || q->river > 0 ) && !q->ocean ){
			q->moisture =
					q->river > 0 ? std::min( 3.0, (double)( 0.2 * q->river ) ) : 1.0;
			queue.push_back( q );
		}else{
			q->moisture = 0.0;
		}
	}
	while( !queue.count ){
		Corner* q = queue.pop();
		ITER_LIST( Corner*, q->adjacent ){
			Corner* r = it->data;
			double newMoisture = q->moisture * 0.9;
			if( newMoisture > r->moisture ){
				r->moisture = newMoisture;
				queue.push_back( r );
			}
		}
	}
	// Salt water
	FOREACH1( q, corners ) {
		if( q->ocean || q->coast )
			q->moisture = 1.0;
	}
}

void MapGenerator::assignPolygonMoisture( )
{
	Center* p;
	FOREACH1( p, centers ) {
		double sumMoisture = 0.0;
		Corner* q;
		FOREACH2( q, corners ) {
			if( q->moisture > 1.0 )
				q->moisture = 1.0;
			sumMoisture += q->moisture;
		}
		p->moisture = sumMoisture / (double)p->corners.count;
	}
}

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
	}else if( p->elevation > 0.8 ){
		if( p->moisture > 0.50 )
			return bSNOW;
		else if( p->moisture > 0.33 )
			return bTUNDRA;
		else if( p->moisture > 0.16 )
			return bBARE;
		else
			return bSCORCHED;
	}else if( p->elevation > 0.6 ){
		if( p->moisture > 0.66 )
			return bTAIGA;
		else if( p->moisture > 0.33 )
			return bSHRUBLAND;
		else
			return bTEMPERATE_DESERT;
	}else if( p->elevation > 0.3 ){
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

void MapGenerator::assignBiomes( )
{
	Center* p;
	FOREACH1( p, centers ) {
		p->biome = getBiome( p );
	}
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

bool MapGenerator::inside( const s2f* p )
{
	return islandShape->call(
			s2f( 2 * ( p->x / SIZE - 0.5 ), 2 * ( p->y / SIZE - 0.5 ) ) );
}

void MapGenerator::printPoints( const char* name )
{
	FILE *fp = fopen( name, "w" );
	Delaunay::Point* p;
	FOREACH1( p, points ) {
		fprintf( fp, "%f %f\n", p->x, p->y );
	}
	fclose( fp );
}

struct ParamsRadial
{
	double ISLAND_FACTOR;
	PM_PRNG islandRandom;
	int bumps;
	double startAngle;
	double dipAngle;
	double dipWidth;
};

bool radial_shape( s2f q, void* prm )
{
	ParamsRadial* param = reinterpret_cast< ParamsRadial* >( prm );
	double angle = atan2( q.y, q.x );
	double p_length = sqrt( q.x * q.x + q.y * q.y );
	double length = 0.5 * ( std::max( abs( q.x ), abs( q.y ) ) + p_length );

	double r1 = 0.5
			+ 0.40
					* sin(
							param->startAngle + param->bumps * angle
									+ cos( ( param->bumps + 3 ) * angle ) );
	double r2 = 0.7
			- 0.20
					* sin(
							param->startAngle + param->bumps * angle
									- sin( ( param->bumps + 2 ) * angle ) );
	if( abs( angle - param->dipAngle ) < param->dipWidth
			|| abs( angle - param->dipAngle + 2 * M_PI ) < param->dipWidth
			|| abs( angle - param->dipAngle - 2 * M_PI ) < param->dipWidth ){
		r1 = r2 = 0.2;
	}
	return ( length < r1 || ( length > r1 * param->ISLAND_FACTOR && length < r2 ) );
}

GenerateFunc* IslandShape::makeRadial( int seed )
{
	GenerateFunc* func = new GenerateFunc();
	ParamsRadial* p = new ParamsRadial();
	p->ISLAND_FACTOR = ISLAND_FACTOR;
	p->islandRandom.seed = seed;
	p->bumps = p->islandRandom.nextIntRange( 1, 6 );
	p->startAngle = p->islandRandom.nextDoubleRange( 0, 2 * M_PI );
	p->dipAngle = p->islandRandom.nextDoubleRange( 0, 2 * M_PI );
	p->dipWidth = p->islandRandom.nextDoubleRange( 0.2, 0.7 );

	func->function = &radial_shape;
	func->param = p;

	return func;
}

bool perlin_shape( s2f q, void* prm )
{
	//var c:Number = (perlin.getPixel(int((q.x+1)*128), int((q.y+1)*128)) & 0xff) / 255.0;
	//	return c > (0.3+0.3*q.length*q.length);
	return true;
}

GenerateFunc* IslandShape::makePerlin( int seed )
{
	GenerateFunc* func = new GenerateFunc();
	func->function = &perlin_shape;
	//var
	//perlin: BitmapData = new BitmapData( 256, 256 );
	//perlin.perlinNoise( 64, 64, 8, seed, false, true );
	//
	return func;
}

bool square_shape( s2f q, void* prm )
{
	return true;
}

GenerateFunc* IslandShape::makeSquare( int seed )
{
	GenerateFunc* func = new GenerateFunc();
	func->function = &square_shape;
	return func;
}

bool blob_shape( s2f q, void* prm )
{
	bool eye1 = sqrt(
			( q.x - 0.2 ) * ( q.x - 0.2 ) + ( q.y / 2 + 0.2 ) * ( q.y / 2 + 0.2 ) )
			< 0.05;
	bool eye2 = sqrt(
			( q.x + 0.2 ) * ( q.x + 0.2 ) + ( q.y / 2 + 0.2 ) * ( q.y / 2 + 0.2 ) )
			< 0.05;
	bool body = sqrt( q.x * q.x + q.y * q.y ) < 0.8 - 0.18 * sin( 5 * atan2( q.y, q.x ) );
	return body && !eye1 && !eye2;
}

GenerateFunc* IslandShape::makeBlob( int seed )
{
	GenerateFunc* func = new GenerateFunc();
	func->function = &blob_shape;
	return func;
}
