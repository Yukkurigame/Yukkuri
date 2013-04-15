/*
 * MapGeneratorShape.cpp
 *
 *  Created on: 12.04.2013
 */

#include "map/generator/MapGenerator.h"
#include "map/generator/modules/Shape.h"
#include "map/generator/Constants.h"
#include "delaunay/delaunay/Voronoi.h"
#include "delaunay/delaunay/Edge.h"
#include "delaunay/geom/Point.h"
#include "utils/misc.h"
#include "hacks.h"
#include "debug.h"
#include <cmath>


namespace MapShape {

	void generateRandomPoints( std::vector< Delaunay::Point* >& pts );
	void improveRandomPoints( std::vector< Delaunay::Point* >& pts );
	void improveCorners( );
	void buildGraph( const std::vector< Delaunay::Point* >& pts, Delaunay::Voronoi* voronoi );
	void generateHeightMap( );
	void assignElevation( );
	void assignOceanCoastAndLand( );
	void assignCornerElevations( );
	void redistributeElevations( list< Corner* >& );
	void assignPolygonElevations( );
	void calculateDownslopes( );

	bool inside( const s2f* p );


	MapProto* proto;
	/**
	 * Island shape is controlled by the islandRandom seed and the
	 * type of island, passed in when we set the island shape. The
	 * islandShape function uses both of them to determine whether any
	 * point should be water or land.
	 */
	ShapeGenerateFunc* islandShape;
	HeightMap heights;

	GeneratorModule module = {	&init, &clean, &process	};
}

void MapShape::init()
{
	islandShape = NULL;
	heights.data = NULL;
	proto = NULL;
}


void MapShape::clean()
{
	islandShape = NULL;
	proto = NULL;
	if( islandShape )
		delete islandShape;
	if( heights.data ){
		for( int i = 0; i < heights.width; ++i )
			delete[] heights.data[i];
		delete[] heights.data;
	}
}


void MapShape::process( MapProto* pr )
{
	proto = pr;
	if( islandShape )
		delete islandShape;
	islandShape = IslandShape::getShape( proto->type, &proto->mapRandom );


	std::vector< Delaunay::Point* > points;

	Debug::debug( Debug::MAP, "Place points...\n" );
	generateRandomPoints( points );

	Debug::debug( Debug::MAP, "Improve points...\n" );
	improveRandomPoints( points );

	/**
	 * Create a graph structure from the Voronoi edge list. The
	 * methods in the Voronoi object are somewhat inconvenient for
	 * my needs, so I transform that data into the data I actually
	 * need: edges connected to the Delaunay triangles and the
	 * Voronoi polygons, a reverse map from those four points back
	 * to the edge, a map from these four points to the points
	 * they connect to (both along the edge and crosswise).
	 */
	Debug::debug( Debug::MAP, "Build graph...\n" );
	Delaunay::Rectangle r( 0, 0, GENERATOR_SIZE, GENERATOR_SIZE );
	Delaunay::Voronoi* voronoi = new Delaunay::Voronoi( points, NULL, r );
	buildGraph( points, voronoi );
	improveCorners();
	FOREACHIT1( points )
		( *it )->dispose();
	points.clear();
	delete voronoi;


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
	//redistributeElevations( cnrs );

	// Assign elevations to non-land corners
	//Corner* q;
	//FOREACH1( q, corners ) {
	//	if( q->ocean || q->coast )
	//		q->elevation = 0.0;
	//}

	// Polygon elevations are the average of their corners
	assignPolygonElevations();

	// Determine downslope paths.
	calculateDownslopes();
}



GeneratorModule* MapShape::get_module()
{
	return &module;
}


/**
 * Generate random points and assign them to be on the island or
 * in the water. Some water points are inland lakes; others are
 * ocean. We'll determine ocean later by looking at what's
 * connected to ocean.
 */
void MapShape::generateRandomPoints( std::vector< Delaunay::Point* >& pts )
{

	for( int i = 0; i < GENERATOR_NUM_POINTS; i++ ){
		Delaunay::Point* p = Delaunay::Point::create(
				proto->mapRandom.nextDouble( 10.0f, GENERATOR_SIZE - 10.0f ),
				proto->mapRandom.nextDouble( 10.0f, GENERATOR_SIZE - 10.0f ) );
		pts.push_back( p );
	}
}


/**
 * Improve the random set of points with Lloyd Relaxation.
 */
void MapShape::improveRandomPoints( std::vector< Delaunay::Point* >& pts )
{

	/**
	 *  We'd really like to generate "blue noise". Algorithms:
	 *  1. Poisson dart throwing: check each new point against all
	 *     existing points, and reject it if it's too close.
	 *  2. Start with a hexagonal grid and randomly perturb points.
	 *  3. Lloyd Relaxation: move each point to the centroid of the
	 *     generated Voronoi polygon, then generate Voronoi again.
	 *  4. Use force-based layout algorithms to push points away.
	 *  5. More at http://www.cs.virginia.edu/~gfx/pubs/antimony/
	 *  Option 3 is implemented here. If it's run for too many iterations,
	 *  it will turn into a grid, but convergence is very slow, and we only
	 *  run it a few times.
	 */

	Delaunay::Rectangle size( 0, 0, GENERATOR_SIZE, GENERATOR_SIZE );
	for( int i = 0; i < GENERATOR_NUM_LLOYD_ITERATIONS; i++ ){
		Delaunay::Voronoi* voronoi = new Delaunay::Voronoi( pts, NULL, size );
		Delaunay::Point* p = NULL;
		FOREACH1( p, pts ) {
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
}


/**
 *  Although Lloyd relaxation improves the uniformity of polygon
 *  sizes, it doesn't help with the edge lengths. Short edges can
 *  be bad for some games, and lead to weird artifacts on
 *  rivers. We can easily lengthen short edges by moving the
 *  corners, but **we lose the Voronoi property**.  The corners are
 *  moved to the average of the polygon centers around them. Short
 *  edges become longer. Long edges tend to become shorter. The
 *  polygons tend to be more uniform after this step.
 */
void MapShape::improveCorners( )
{
	std::map< int, s2f > newCorners;

	// First we compute the average of the centers next to each corner.
	Corner* q = NULL;
	FOREACH1( q, proto->corners ) {
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
	for( UINT i = 0; i < proto->corners.size(); i++ ){
		proto->corners[i]->point = newCorners[i];
	}


	// The edge midpoints were computed for the old corners and need
	// to be recomputed.
	Edge* edge;
	FOREACH1( edge, proto->edges ) {
		if( edge->v0 && edge->v1 )
			edge->midpoint = interpolate( edge->v0->point, edge->v1->point, 0.5 );
	}
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



/**
 * Build graph data structure in 'edges', 'centers', 'corners',
 * based on information in the Voronoi results: point.neighbors
 * will be a list of neighboring points of the same type (corner
 * or center); point.edges will be a list of edges that include
 * that point. Each edge connects to four points: the Voronoi edge
 * edge.{v0,v1} and its dual Delaunay triangle edge edge.{d0,d1}.
 * For boundary polygons, the Delaunay edge will have one null
 * point, and the Voronoi edge may be null.
 */
// FIXME: M-M-MEGASLOW
void MapShape::buildGraph( const std::vector< Delaunay::Point* >& pts,
		Delaunay::Voronoi* voronoi )
{
	std::map< float, std::map< float, Center* > > centerLookup;

	// Build Center objects for each of the points, and a lookup map
	// to find those Center objects again as we build the graph
	Delaunay::Point* point;
	FOREACH1( point, pts ){
		Center* p = new Center();
		p->index = proto->centers.size();
		p->point = s2f(point->x, point->y);
		proto->centers.push_back(p);
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

		edge->index = proto->edges.size();
		edge->river = 0;
		proto->edges.push_back( edge );
		if( vedge->p0 && vedge->p1 ){
			s2f vp0( vedge->p0->x, vedge->p0->y );
			s2f vp1( vedge->p1->x, vedge->p1->y );
			edge->midpoint = interpolate( vp0, vp1, 0.5 );
		}

		// Edges point to corners. Edges point to centers.
		edge->v0 = makeCorner( proto->corners, _cornerMap, vedge->p0, GENERATOR_SIZE );
		edge->v1 = makeCorner( proto->corners, _cornerMap, vedge->p1, GENERATOR_SIZE );
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



void MapShape::generateHeightMap( )
{
	ShapeHeightGenerateFunc* f = IslandShape::defaultHeight( &proto->mapRandom );
	IslandShape::diamondSquare( &heights, GENERATOR_SIZE, f );
	delete f;
}


/**
 * Determine polygon and corner types: ocean, coast, land.
 */
void MapShape::assignOceanCoastAndLand( )
{
	/**
	 * Compute polygon attributes 'ocean' and 'water' based on the
	 * corner attributes. Count the water corners per
	 * polygon. Oceans are all polygons connected to the edge of the
	 * map. In the first pass, mark the edges of the map as ocean;
	 * in the second pass, mark any water-containing polygon or
	 * polygon with less elevation connected an ocean as ocean.
	 */
	list< Center* > queue;
	Center* p;

	FOREACH1( p, proto->centers ) {
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
		p->water = ( p->ocean || numWater >= p->corners.count * GENERATOR_LAKE_THRESHOLD );
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
	FOREACH1( p, proto->centers ) {
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
	FOREACH1( q, proto->corners ) {
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


void MapShape::assignElevation( )
{
	// TODO: for more realistic landscape with smooth valleys and rough peaks
	// we need to generate 2 maps, normalize it and multiply.
	Corner* q;
	FOREACH1( q, proto->corners ){
		int x = q->point.x;
		int y = q->point.y;
		q->elevation = ( heights.data[x][y] - heights.min ) / ( heights.max - heights.min );
	}
}


/**
 * Determine whether a given point should be on the island or in the water.
 */
bool MapShape::inside( const s2f* p )
{
	return islandShape->call(
			s2f( 2 * ( p->x / GENERATOR_SIZE - 0.5 ), 2 * ( p->y / GENERATOR_SIZE - 0.5 ) ) );
}



/**
 * Determine elevations and water at Voronoi corners. By
 * construction, we have no local minima. This is important for
 * the downslope vectors later, which are used in the river
 * construction algorithm. Also by construction, inlets/bays
 * push low elevation areas inland, which means many rivers end
 * up flowing out through them. Also by construction, lakes
 * often end up on river paths because they don't raise the
 * elevation as much as other terrain does.
 */
void MapShape::assignCornerElevations( )
{
	list< Corner* > queue;

	// Make island?
	Corner* q;
	FOREACH1( q, proto->corners ) {
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


/**
 * Change the overall distribution of elevations so that lower
 * elevations are more common than higher
 * elevations. Specifically, we want elevation X to have frequency
 * (1-X).  To do this we will sort the corners, then set each
 * corner to its desired elevation.
 */
void MapShape::redistributeElevations( list< Corner* >& locations )
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


/**
 * Polygon elevations are the average of the elevations of their corners.
 */
void MapShape::assignPolygonElevations( )
{
	Center* p;
	FOREACH1( p, proto->centers ) {
		double sumElevation = 0.0;
		ITER_LIST( Corner*, p->corners ) {
			sumElevation += it->data->elevation;
		}
		p->elevation = (double)sumElevation / (double)p->corners.count;
	}
}



/**
 * Calculate downslope pointers.  At every point, we point to the
 * point downstream from it, or to itself.  This is used for
 * generating rivers and watersheds.
 */
void MapShape::calculateDownslopes( )
{
	Corner* q;
	FOREACH1( q, proto->corners ) {
		Corner* r = q;
		ITER_LIST( Corner*, q->adjacent ){
			if( it->data->elevation <= r->elevation )
				r = it->data;
		}
		q->downslope = r;
	}
}
