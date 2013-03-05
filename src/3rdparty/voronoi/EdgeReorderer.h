/*
 * EdgeReorderer.h
 *
 *  Created on: 05.03.2013
 *
 */

#ifndef EDGEREORDERER_H_
#define EDGEREORDERER_H_

#include "3rdparty/voronoi/VoronoiExternal.h"

class EdgeReorderer
{
public:
	EdgeReorderer( VoronoiEdge** origEdges, int length );
	~EdgeReorderer( );

	VoronoiEdge** edges( )
	{
		return edges_list;
	}

	short* edgeOrientations( )
	{
		return edge_orientations;
	}


private:
	VoronoiEdge** edges_list;
	short* edge_orientations;

	void reorderEdges( VoronoiEdge**, int );

};

#endif /* EDGEREORDERER_H_ */
