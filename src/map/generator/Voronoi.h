/*
 * Voronoi.h
 *
 *  Created on: 02.03.2013
 */

#ifndef VORONOI_H_
#define VORONOI_H_

#include <vector>
#include <map>
#include "math.h"
#include "basic_types.h"

struct SourcePoint;


inline static double distance( const s2f* v1, const s2f* v2 )
{
	s2f v( v1->x - v2->x, v1->y - v2->y );
	return sqrt( v.x * v.x + v.y * v.y );
}

struct LineSegment
{
	const s2f* p0;
	const s2f* p1;

	LineSegment( const s2f* pt0, const s2f* pt1 ) :
			p0( pt0 ), p1( pt1 )
	{
	}

	double compareLengths_MAX( const LineSegment& segment0, const LineSegment& segment1 )
	{
		double length0 = distance( segment0.p0, segment0.p1 );
		double length1 = distance( segment1.p0, segment1.p1 );
		if( length0 < length1 )
			return 1;
		if( length0 > length1 )
			return -1;
		return 0;
	}

	double compareLengths( const LineSegment& edge0, const LineSegment& edge1 )
	{
		return -compareLengths_MAX( edge0, edge1 );
	}
};

struct Line
{
	s2f start;
	s2f end;
	bool visible;

	Line( float x1, float y1, float x2, float y2 ) :
			start( x1, y1 ), end( x2, y2 ), visible( true )
	{
	}

	LineSegment delaunayLine( ) const
	{
		// draw a line connecting the input Sites for which the edge is a bisector:
		return LineSegment( &start, &end );
	}

	LineSegment voronoiEdge( ) const
	{
		if( !visible )
			return LineSegment( NULL, NULL );
		//return new LineSegment(_clippedVertices[LR.LEFT],
		//			_clippedVertices[LR.RIGHT]);
		return LineSegment( &start, &end );
	}
};

class Voronoi
{

public:
	Voronoi( const std::vector<s2f>& points, const rect2f& size, int min_dist );
	~Voronoi( );

	const std::vector<Line>& lines( );


private:
	void addSites( const std::vector<s2f>& points );

	UINT sites_count;
	SourcePoint* sites;
	std::map< const s2f*, SourcePoint* > sitesByLocation;
	std::vector<Line> Lines;

};

#endif /* VORONOI_H_ */
