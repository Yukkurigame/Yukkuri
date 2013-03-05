/*
 * VoronoiExternal.cpp
 *
 *  Created on: 05.03.2013
 *
 */

#include "3rdparty/voronoi/VoronoiExternal.h"
#include "3rdparty/voronoi/EdgeReorderer.h"


void site_add_egde( Site* s, VoronoiEdge* e )
{
	s->edges_count++;
	s->edges = (VoronoiEdge**)realloc( s->edges_count * sizeof(VoronoiEdge*) );
	s->edges[s->edges_count-1] = e;
}


int site_region( Site* s )
{
	if( !s || !s->edges_count )
		return ;

	if( s->edge_orientations == 0 ){
		EdgeReorderer reorderer(s->edges, s->edges_count);
		s->edge_orientations = reorderer.edgeOrientations();

		_region = clipToBounds( clippingBounds );
		if( (new Polygon( _region )).winding( ) == Winding.CLOCKWISE ){
			_region = _region.reverse( );
		}
	}
	return _region;
}


void clipToBounds( float bounds[4] )
{
	var points:Vector.<Point> = new Vector.<Point>;
	var n:int = _edges.length;
	var i:int = 0;
	var edge:Edge;
	while (i < n && ((_edges[i] as Edge).visible == false))
	{
		++i;
	}

	if (i == n)
	{
// no edges visible
		return new Vector.<Point>();
	}
	edge = _edges[i];
	var orientation:LR = _edgeOrientations[i];
	points.push(edge.clippedEnds[orientation]);
	points.push(edge.clippedEnds[LR.other(orientation)]);

	for (var j:int = i + 1; j < n; ++j)
	{
		edge = _edges[j];
		if (edge.visible == false)
		{
			continue;
		}
		connect(points, j, bounds);
	}
// close up the polygon by adding another corner point of the bounds if needed:
	connect(points, i, bounds, true);

	return points;
}
