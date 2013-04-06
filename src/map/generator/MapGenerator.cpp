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
#include <cmath>



MapGenerator::MapGenerator( float size ) : islandShape( NULL )
{
	SIZE = size;
	heights.data = NULL;
	reset();
}

MapGenerator::~MapGenerator( )
{
	reset();
	if( islandShape )
		delete islandShape;
	if( heights.data ){
		for( int i = 0; i < heights.width; ++i )
			delete[] heights.data[i];
		delete[] heights.data;
	}

	clear_vector( &centers );
	clear_vector( &edges );
	clear_vector( &corners );
}

void MapGenerator::newIsland( IslandForm type, int seed )
{
	initial_seed = seed;
	mapRandom.seed( seed );
	latitude = mapRandom.nextDouble( -90.0, 90.0 );
	longitude = mapRandom.nextDouble( -180.0, 180.0 );

	if( islandShape )
			delete islandShape;
		islandShape = IslandShape::getShape( type, &mapRandom );
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
			FOREACHIT1( points )
				(*it)->dispose();
			points.clear();
			delete voronoi;
			CHECK_LAST( 3 )
		}
		case 3:
		{
			Debug::debug( Debug::MAP, "Assign elevations...\n" );
			// Determine the elevations and water at Voronoi corners.
			//assignCornerElevations();

			// Generate heightmap for new location
			generateHeightMap();
			assignElevation( );

			// Determine polygon and corner type: ocean, coast, land.
			assignOceanCoastAndLand();

			// Rescale elevations so that the highest is 1.0, and they're
			// distributed well. We want lower elevations to be more common
			// than higher elevations, in proportions approximately matching
			// concentric rings. That is, the lowest elevation is the
			// largest ring around the island, and therefore should more
			// land area than the highest elevation, which is the very
			// center of a perfectly circular island.
			//list< Corner* > cnrs = landCorners( corners );
			//redistributeElevations( cnrs );


			// Assign elevations to non-land corners
			//Corner* q;
			//FOREACH1( q, corners ) {
			//	if( q->ocean || q->coast )
			//		q->elevation = 0.0;
			//}

			// Polygon elevations are the average of their corners
			assignPolygonElevations();
			CHECK_LAST( 4 )
		}
		case 4:
		{
			Debug::debug( Debug::MAP, "Calculate temperature...\n" );

			assignTemperature();
			redistributeTemperature();

			CHECK_LAST( 5 )
		}
		case 5:
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
			list< Corner* > cnrs = landCorners( corners );
			redistributeMoisture( cnrs );
			assignPolygonMoisture();
			CHECK_LAST( 6 )
		}
		case 6:
		{
			Debug::debug( Debug::MAP, "Decorate map...\n" );
			assignBiomes();
			CHECK_LAST( 7 )
		}
		default:
			break;
	}

}

void MapGenerator::generateRandomPoints( std::vector< Delaunay::Point* >& pts )
{

	for( int i = 0; i < NUM_POINTS; i++ ){
		Delaunay::Point* p = Delaunay::Point::create(
				mapRandom.nextDouble( 10, SIZE - 10 ),
				mapRandom.nextDouble( 10, SIZE - 10 ) );
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
				q->dispose();
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
}

list< Corner* > MapGenerator::landCorners( const std::vector< Corner* >& c )
{
	list< Corner* > locations;
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

		dedge->dispose();
		vedge->dispose();

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

void MapGenerator::generateHeightMap( )
{
	ShapeHeightGenerateFunc* f = IslandShape::defaultHeight( &mapRandom );
	IslandShape::diamondSquare( &heights, SIZE, f );
	delete f;
}

void MapGenerator::assignElevation( )
{
	// TODO: for more realistic landscape with smooth valleys and rough peaks
	// we need to generate 2 maps, normalize it and multiply.
	Corner* q;
	FOREACH1( q, corners ){
		int x = q->point.x;
		int y = q->point.y;
		q->elevation = ( heights.data[x][y] - heights.min ) / ( heights.max - heights.min );
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

void MapGenerator::redistributeElevations( list< Corner* >& locations )
{
	// SCALE_FACTOR increases the mountain area. At 1.0 the maximum
	// elevation barely shows up on the map, so we set it to 1.1.
	const double SCALE_FACTOR = 1.1;
	locations.sort( compareElevations );
	float length = locations.count;
	int index = 0;
	ITER_LIST( Corner*, locations ){
		// Let y(x) be the total area that we want at elevation <= x.
		// We want the higher elevations to occur less than lower
		// ones, and set the area to be y(x) = 1 - (1-x)^2.
		double y = (double)index / length;
		// Now we have to solve for x, given the known y.
		//  *  y = 1 - (1-x)^2
		//  *  y = 1 - (1 - 2x + x^2)
		//  *  y = 2x - x^2
		//  *  x^2 - 2x + y = 0
		// From this we can use the quadratic equation to get:
		double x = sqrt( SCALE_FACTOR ) - sqrt( SCALE_FACTOR * ( 1.0 - y ) );
		if( x > 1.0 )
			x = 1.0;  // TODO: does this break downslopes?
		it->data->elevation = x;
		index++;
	}
}

/* Calculate temperature map for generated region
 * Rules of model:
 * All nodes get default region temperature.
 * - Default region temperature is based on geographical position.
 * - Each degree of latitude lowers temperature by 0.75 degree in direction from equator to pole.
 * - Biomes calculation must take into account annual temperature variations.
 * Individual nodes temperature.
 * - Depends on height. Each unit of height lowers temperature by 3 degree
 * - Depends of ocean position. Each border adjacent with ocean gives +5 degree to node,
 *   second-neighborhood gives +2 degree.
 * - Ocean influence must depend on region temperature. It must lower temperature for hot regions.
 * - Flatness of nodes. Bunch of nodes with slightly difference in height must receive
 *   overall delta temperature bonus.
 */
void MapGenerator::assignTemperature( )
{
	float temperature = 35.0 - abs(latitude) * 0.75;

	Corner* q;
	FOREACH1( q, corners ) {
		// Minimum temperature: 35 - 0.75 * 90 - 40 * 0.7 = -60.5 degree.
		q->temperature = temperature - ( q->elevation - 0.3 ) * 40;

		// Ocean normalizes temperature to some limit
		if( q->ocean ){
			if( q->temperature < 5 )
				q->temperature += 20;
			continue;
		}
	}

	// Assign calculated temperature to centers
	Center* p;
	FOREACH1( p, centers ) {
		float temperature = 0.0;
		float corners = p->corners.count;
		ITER_LIST(Corner*, p->corners ) {
			temperature += it->data->temperature;
		}
		p->temperature = temperature / corners;
	}
}

/* Make temperature field more smooth */
void MapGenerator::redistributeTemperature( )
{
/*

	// Coasts have ocean influence
	float first_neighbor = 0;
	float second_neighbor = 0;
	ITER_LIST( Center*, q->touches ){
		Center* c = it->data;
		if( c->ocean ){
			first_neighbor += 5;
			continue;
		}

		// search for second neighbor
		ITER_LIST2( Center*, c->neighbors ){
			Center* n = it->data;
			if( n->ocean )
				second_neighbor += 2;
		}
	}

	float first_temp = q->temperature + first_neighbor;
	q->temperature = ( first_temp > 35 ? 35 : first_temp );

	// Second neighborhood may give more heat.
	q->temperature += second_neighbor;
*/
}

bool compareMoisture( const Corner* a, const Corner* b )
{
	return a->moisture < b->moisture;
}

void MapGenerator::redistributeMoisture( list< Corner* >& locations )
{
	locations.sort( compareMoisture );
	float length = locations.count;
	int index = 0;
	ITER_LIST( Corner*, locations ){
		double m = (double)index / length;
		it->data->moisture = m;
		index++;
	}
}

void MapGenerator::assignOceanCoastAndLand( )
{
	// Compute polygon attributes 'ocean' and 'water' based on the
	// corner attributes. Count the water corners per
	// polygon. Oceans are all polygons connected to the edge of the
	// map. In the first pass, mark the edges of the map as ocean;
	// in the second pass, mark any water-containing polygon or
	// polygon with less elevation connected an ocean as ocean.
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
	while( queue.count > 0 ){
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
	for( int i = 0, changed = 0; i < 100; ++i, changed = 0 ){
		Corner* q;
		FOREACH1( q, corners ) {
			if( !q->ocean && !q->coast && !q->watershed->coast ){
				Corner* r = q->downslope->watershed;
				//Corner* r = q->downslope->watershed;
				if( !r->ocean )
					q->watershed = r;
				changed++;
			}
		}
		if( !changed )
			break;
	}

	// How big is each watershed?
	FOREACH1( q, corners ) {
		Corner* r = q->watershed;
		r->watershed_size++;
	}

	int min_watershed = 20;
	for( int i = 0, changed = 0; i < 100; ++i, changed = 0 ){
		FOREACH1( q, corners ) {
			if( q->watershed->watershed_size > min_watershed )
				continue;
			ITER_LIST( Corner*, q->adjacent ){
				Corner* r = it->data->watershed;
				if( r->watershed_size > min_watershed ){
					q->watershed = r->watershed;
					changed = true;
					break;
				}
			}
		}
		if( !changed )
			break;
	}
}

void MapGenerator::createRivers( )
{
	int length = corners.size();
	if( !length )
		return;

	list< Corner* > river_sources;

	for( int i = 0; i < SIZE / 2; i++ ){
		Corner* q = corners[mapRandom.nextInt( 0, length - 1 )];
		if( q->ocean || q->elevation < 0.3 || q->elevation > 0.9 )
			continue;

		river_sources.push(q);

		// Bias rivers to go west: if (q.downslope.x > q.x) continue;
		while( !q->coast ){
			if( q == q->downslope )
				break;
			Edge* edge = lookupEdgeFromCorner( q, q->downslope );
			edge->river++;
			q->river++;
			q->downslope->river++; // TODO: fix double count
			q = q->downslope;
		}
	}

	// Make all rivers end at lakes or another rivers
	while( river_sources.count > 0 ){
		Corner* source = river_sources.pop();
		Corner* q = source;
		while( !q->coast && q != q->downslope && q->downslope->river )
			q = q->downslope;
		if( q->coast )
			continue;
		if( q->river > 3 )
			q->downslope->touches.head->data->water = true;
		// TODO: connect to another rivers
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
					q->river > 0 ? std::min( 3.0, 0.2 * (double)q->river ) : 1.0;
			queue.push_back( q );
		}else{
			q->moisture = 0.0;
		}
	}

	while( queue.count > 0 ){
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
		double count = p->corners.count;
		ITER_LIST( Corner*, p->corners ){
			Corner* q = it->data;
			if( q->moisture > 1.0 )
				q->moisture = 1.0;
			sumMoisture += q->moisture;
		}
		p->moisture = sumMoisture / count;
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

