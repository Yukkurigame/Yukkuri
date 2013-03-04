/*
 * Edge.h
 *
 *  Created on: 02.03.2013
 */

#ifndef EDGE_H_
#define EDGE_H_

#include "map/generator/graph/Center.h"
#include "map/generator/graph/Corner.h"
#include "basic_types.h"


class Edge {
public:
	Edge( );
	virtual ~Edge( );

	int index;
	Center* d0;	// Delaunay edge
	Center* d1; // Delaunay edge
	Corner* v0; // Voronoi edge
	Corner* v1; // Voronoi edge
	s2f midpoint; // halfway between v0,v1
	int river;  // volume of water, or 0

};

#endif /* EDGE_H_ */
