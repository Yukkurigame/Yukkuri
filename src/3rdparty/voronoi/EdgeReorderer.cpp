/*
 * EdgeReorderer.cpp
 *
 *  Created on: 05.03.2013
 *
 */

#include "3rdparty/voronoi/EdgeReorderer.h"
#include "utils/list.h"
#include <stdlib.h>
#include <string.h>


EdgeReorderer::EdgeReorderer( VoronoiEdge** origEdges, int length ) :
		edges_list( 0 ), edge_orientations( 0 )
{
	edges_list = origEdges;
	if( length > 0 )
		reorderEdges( origEdges, length );
}

EdgeReorderer::~EdgeReorderer( )
{
	// TODO Auto-generated destructor stub
}

void EdgeReorderer::reorderEdges( VoronoiEdge** origEdges, int length )
{
	list<VoronoiEdge*> newEdges;
	list<short> _edge_orientations;

	int sh_length = sizeof(short) * length;

	edge_orientations = (short*)malloc( sh_length );
	memset( edge_orientations, 0, sh_length );

	// we're going to reorder the edges in order of traversal
	short* done = (short*)malloc( sh_length );
	memset( done, 0, sh_length );

	int nDone = 0;
	VoronoiEdge* edge = origEdges[0];
	newEdges.push_back( edge );
	_edge_orientations.push_back( ssLeft );

	Point* firstPoint = &edge->reg[ssLeft]->coord;
	Point* lastPoint = &edge->reg[ssRight]->coord;

	//if (firstPoint == Vertex.VERTEX_AT_INFINITY || lastPoint == Vertex.VERTEX_AT_INFINITY)
	//{
	//	return new Vector.<Edge>();
	//}

	done[0] = 1;
	++nDone;

	while( nDone < length ){
		for( int i = 1; i < length; ++i ){
			if( done[i] )
				continue;

			edge = origEdges[i];
			Point* leftPoint = &edge->reg[ssLeft]->coord;
			Point* rightPoint = &edge->reg[ssRight]->coord;
			//if( leftPoint == Vertex.VERTEX_AT_INFINITY	|| rightPoint == Vertex.VERTEX_AT_INFINITY ){
			//	return new Vector.<Edge>();
			//}
			if( leftPoint == lastPoint ){
				lastPoint = rightPoint;
				_edge_orientations.push_back( ssLeft );
				newEdges.push_back( edge );
				done[i] = 1;
			}else if( rightPoint == firstPoint ){
				firstPoint = leftPoint;
				_edge_orientations.push( ssLeft );
				newEdges.push( edge );
				done[i] = 1;
			}else if( leftPoint == firstPoint ){
				firstPoint = rightPoint;
				_edge_orientations.push( ssRight );
				newEdges.push( edge );
				done[i] = 1;
			}else if( rightPoint == lastPoint ){
				lastPoint = leftPoint;
				_edge_orientations.push_back( ssRight );
				newEdges.push_back( edge );
				done[i] = 1;
			}
			if( done[i] )
				++nDone;
		}
	}

	free( done );

	listElement<VoronoiEdge*>* eel = newEdges.head;
	listElement<short>* oel = _edge_orientations.head;

	for( int v = 0; v < length; ++v ){
		origEdges[v] = eel->data;
		edge_orientations[v] = oel->data;
		eel = eel->next;
		oel = oel->next;
	}
}
