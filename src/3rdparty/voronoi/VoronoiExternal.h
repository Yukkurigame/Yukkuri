/*
 * VoronoiExternal.h
 *
 *  Created on: 04.03.2013
 */

#ifndef VORONOIEXTERNAL_H_
#define VORONOIEXTERNAL_H_

#include <cmath>

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

struct VoronoiEdge;

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
	short* edge_orientations;
	Point** region;
	int region_size;
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


enum SiteSide {
	ssLeft = 0, ssRight, ssTop, ssBottom
};

enum BoundsCheckStates
{
	bcTOP = 0x01, bcBOTTOM = 0x02, bcLEFT = 0x04, bcRIGHT = 0x08
};

void site_add_egde( Site*, VoronoiEdge* );
void site_region( Site*, float[4] );

inline static double distance( const Point* v1, const Point* v2 )
{
	return sqrt((v1->x - v2->x) * (v1->x - v2->x) + (v1->y - v2->y) * (v1->y - v2->y));
}

// bounds: xmin xmax ymin ymax
inline int check_bounds( Point* point, float bounds[4] )
{
	int value = 0;
	if( point->x == bounds[ssLeft] )
		value |= bcLEFT;
	if( point->x == bounds[ssRight] )
		value |= bcRIGHT;
	if( point->y == bounds[ssTop] )
		value |= bcTOP;
	if( point->y == bounds[ssBottom] )
		value |= bcBOTTOM;
	return value;
}

#endif /* VORONOIEXTERNAL_H_ */
