/*
 * VoronoiExternal.h
 *
 *  Created on: 04.03.2013
 */

#ifndef VORONOIEXTERNAL_H_
#define VORONOIEXTERNAL_H_

struct SourcePoint
{
	int id;
	double weight;
	double x;
	double y;
};

struct Point
{
	float x, y;
};

// structure used both for sites and for vertices
struct Site
{
	struct Point coord;
	struct Point coordout;
	double weight;
	int sitenbr;
	int refcnt;
	int edges_count;
	VoronoiEdge** edges;
	Point** region;
	short* edge_orientations;
};

enum SiteSide {
	ssLeft = 0, ssRight
};

// External edge handler
struct VoronoiEdge
{
	float a, b, c;
	struct Site *ep[2];
	struct Site *reg[2];
	int index;
	struct Point clipped[2];
	bool visible;
};


void site_add_egde( Site*, VoronoiEdge* );
int site_region( Site* );


#endif /* VORONOIEXTERNAL_H_ */
