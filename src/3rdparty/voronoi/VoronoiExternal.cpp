/*
 * VoronoiExternal.cpp
 *
 *  Created on: 05.03.2013
 *
 */

#include "3rdparty/voronoi/VoronoiExternal.h"
#include "3rdparty/voronoi/EdgeReorderer.h"
#include "utils/list.h"

#include <stdlib.h>


#define EPSILON 0.005

static list<Point*> points_pool;

bool closeEnough( const Point* p0, const Point* p1 )
{
	return distance( p0, p1 ) < EPSILON;
}

Point* new_point( float x, float y )
{
	Point* p = new Point;
	p->x = x;
	p->y = y;
	points_pool.push_back( p );
	return p;
}

void site_add_egde( Site* s, VoronoiEdge* e )
{
	s->edges_count++;
	s->edges = (VoronoiEdge**)realloc( s->edges, s->edges_count * sizeof(VoronoiEdge*) );
	s->edges[s->edges_count - 1] = e;
}



// TODO: merge to one
inline void check_vside( bool right, float bounds[4], int& newCheck, list<Point*>& points,
		Point*& rightPoint, Point*& newPoint )
{
	float px = bounds[right ? ssRight : ssLeft];
	float py = 0;
	if( newCheck & bcBOTTOM ){
		py = bounds[ssBottom];
		points.push_back( new_point( px, py ) );
	}else if( newCheck & bcTOP ){
		py = bounds[ssTop];
		points.push_back( new_point( px, py ) );
	}else if( newCheck & ( right ? bcLEFT : bcRIGHT ) ){
		int bsize = bounds[ssTop] - bounds[ssBottom];
		int pts_sz = rightPoint->y + newPoint->y - 2 * bounds[ssTop];
		py = bounds[( pts_sz < bsize ) ? ssTop : ssBottom];
		points.push_back( new_point( px, py ) );
		points.push_back( new_point( bounds[right ? ssLeft : ssRight], py ) );
	}
}

inline void check_hside( bool top, float bounds[4], int& newCheck, list<Point*>& points,
		Point*& rightPoint, Point*& newPoint )
{
	float px = 0;
	float py = bounds[top ? ssTop : ssBottom];
	if( newCheck & bcRIGHT ){
		px = bounds[ssRight];
		points.push_back( new_point( px, py ) );
	}else if( newCheck & bcLEFT ){
		px = bounds[ssLeft];
		points.push_back( new_point( px, py ) );
	}else if( newCheck & ( top ? bcBOTTOM : bcTOP ) ){
		int bsize = bounds[ssRight] - bounds[ssLeft];
		int pts_sz = rightPoint->x + newPoint->x - 2 * bounds[ssLeft];
		px = bounds[pts_sz < bsize ? ssLeft : ssRight];
		points.push_back( new_point( px, py ) );
		points.push_back( new_point( px, bounds[top ? ssBottom : ssTop] ) );
	}
}

void connect( Site* s, list<Point*>& points, int j, float bounds[4],
		bool closingUp = false )
{
	Point* rightPoint = points.tail->data;
	VoronoiEdge* newEdge = s->edges[j];
	short newOrientation = s->edge_orientations[j];

	// the point that  must be connected to rightPoint:
	Point* newPoint = &newEdge->clipped[newOrientation];
	if( !closeEnough( rightPoint, newPoint ) ){
		// The points do not coincide, so they must have been clipped at the bounds;
		// see if they are on the same border of the bounds:
		if( rightPoint->x != newPoint->x && rightPoint->y != newPoint->y ){
			// They are on different borders of the bounds;
			// insert one or two corners of bounds as needed to hook them up:
			// (NOTE this will not be correct if the region should take up more than
			// half of the bounds rect, for then we will have gone the wrong way
			// around the bounds and included the smaller part rather than the larger)
			int rightCheck = check_bounds( rightPoint, bounds );
			int newCheck = check_bounds( newPoint, bounds );
			if( rightCheck & bcRIGHT ){
				check_vside( true, bounds, newCheck, points, rightPoint, newPoint );
			}else if( rightCheck & bcLEFT ){
				check_vside( false, bounds, newCheck, points, rightPoint, newPoint );
			}else if( rightCheck & bcTOP ){
				check_hside( true, bounds, newCheck, points, rightPoint, newPoint );
			}else if( rightCheck & bcBOTTOM ){
				check_hside( false, bounds, newCheck, points, rightPoint, newPoint );
			}
		}
		if( closingUp ) // newEdge's ends have already been added
			return;
		points.push_back( newPoint );
	}
	Point* newRightPoint = &newEdge->clipped[
			( newOrientation == ssLeft ) ? ssRight : ssLeft];
	if( !closeEnough( points.head->data, newRightPoint ) )
		points.push_back( newRightPoint );
}


void clipToBounds( Site* s, float bounds[4] )
{

	if( s->region )
		free( s->region );

	s->region = 0;
	s->region_size = 0;

	int index = 0;
	int elength = s->edges_count;

	while( index < elength && ( !s->edges[index]->visible ) )
		++index;

	if( index == elength ) // no edges visible
		return;

	int viscount = index;
	while( viscount < elength )
		if( s->edges[index]->visible )
			++viscount;

	VoronoiEdge* edge = s->edges[index];
	short orientation = s->edge_orientations[index];
	short rev_o = ( orientation == ssLeft ? ssRight : ssLeft );

	list<Point*> points;
	points.push_back( &edge->clipped[orientation] );
	points.push_back( &edge->clipped[rev_o] );

	for( int j = index + 1; j < elength; ++j ){
		edge = s->edges[j];
		if( !edge->visible )
			continue;
		connect( s, points, j, bounds );
	}

	// close up the polygon by adding another corner point of the bounds if needed:
	connect( s, points, index, bounds, true );

	// Check if we need to reverse list
	double signedDoubleArea = 0;
	ITER_LIST( Point*, points ) {
		Point* point = it->data;
		Point* next = ( it->next ? it->next->data : points.head->data );
		signedDoubleArea += point->x * next->y - next->x * point->y;
	}
	bool reverse = ( signedDoubleArea < 0 );

	s->region_size = points.count;
	s->region = (Point**)malloc( points.count * sizeof(Point*) );

	// Place points to region
	int idx = reverse ? points.count - 1 : 0;
	ITER_LIST( Point*, points ) {
		s->region[idx] = it->data;
		idx += reverse ? -1 : 1;
	}
}


void site_region( Site* s, float bb[4] )
{
	if( !s || !s->edges_count )
		return;

	if( s->edge_orientations == 0 ){
		EdgeReorderer reorderer( s->edges, s->edges_count );
		s->edge_orientations = reorderer.edgeOrientations();

		clipToBounds( s, bb );
	}
}
