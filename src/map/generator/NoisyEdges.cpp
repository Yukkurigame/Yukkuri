/*
 * NoisyEdges.cpp
 *
 *  Created on: 08.03.2013
 */

#include "NoisyEdges.h"
#include "hacks.h"
#include <math.h>

NoisyEdges::NoisyEdges( )
{
	// TODO Auto-generated constructor stub

}

NoisyEdges::~NoisyEdges( )
{
	// TODO Auto-generated destructor stub
	FOREACHIT1( path0 ){
		delete it->second;
	}
	path0.clear();

	FOREACHIT1( path1 ){
		delete it->second;
	}
	path1.clear();
}

void NoisyEdges::buildNoisyEdges( std::vector< Center* >& centers, /*Lava lava,*/PM_PRNG* random )
{

	Center* p = NULL;
	FOREACH1( p, centers ) {
		ITER_LIST(	Edge*, p->borders ){
			Edge* edge = it->data;
			if( !edge->d0 || !edge->d1 || !edge->v0 || !edge->v1
					|| path0.count( edge->index ) > 0 )
				continue;
			s2f t = interpolate( edge->v0->point, edge->d0->point, NOISY_LINE_TRADEOFF );
			s2f q = interpolate( edge->v0->point, edge->d1->point, NOISY_LINE_TRADEOFF );
			s2f r = interpolate( edge->v1->point, edge->d0->point, NOISY_LINE_TRADEOFF );
			s2f s = interpolate( edge->v1->point, edge->d1->point, NOISY_LINE_TRADEOFF );
			int minl = 10;
			if( edge->d0->biome != edge->d1->biome )
				minl = 3;
			if( edge->d0->ocean && edge->d1->ocean )
				minl = 100;
			if( edge->d0->coast || edge->d1->coast )
				minl = 1;
			//if (edge->river || lava.lava[edge.index]) minLength = 1;

			path0[edge->index] = buildNoisyLineSegments( random, &edge->v0->point, &t,
					&edge->midpoint, &q, minl );
			path1[edge->index] = buildNoisyLineSegments( random, &edge->v1->point, &s,
					&edge->midpoint, &r, minl );
		}
	}
}

struct Closure
{
	list< s2f >* points;
	PM_PRNG* random;
	float min_length;
};

float length( const s2f& v )
{
	return sqrt( v.x * v.x + v.y * v.y );
}

void subdivide( const s2f* a, const s2f* b, const s2f* c, const s2f* d, Closure& cl )
{
	const s2f& A = *a;
	const s2f& B = *b;
	const s2f& C = *c;
	const s2f& D = *d;

	if( length( A - C ) < cl.min_length || length( B - D ) < cl.min_length )
		return;

	// Subdivide the quadrilateral
	double p = cl.random->nextDoubleRange( 0.2, 0.8 );  // vertical (along A-D and B-C)
	double q = cl.random->nextDoubleRange( 0.2, 0.8 );  // horizontal (along A-B and D-C)

	// Midpoints
	s2f E = interpolate( A, D, p );
	s2f F = interpolate( B, C, p );
	s2f G = interpolate( A, B, q );
	s2f I = interpolate( D, C, q );

	// Central point
	s2f H = interpolate( E, F, q );

	// Divide the quad into subquads, but meet at H
	float s = 1.0 - cl.random->nextDoubleRange( -0.4, +0.4 );
	float t = 1.0 - cl.random->nextDoubleRange( -0.4, +0.4 );
	s2f sb = interpolate( G, B, s );
	s2f sd = interpolate( E, D, t );
	subdivide( a, &sb, &H, &sd, cl );
	cl.points->push_back( H );
	sb = interpolate( F, C, s );
	sd = interpolate( I, D, t );
	subdivide( &H, &sb, c, &sd, cl );
}

list< s2f >* NoisyEdges::buildNoisyLineSegments( PM_PRNG* rndm, const s2f* A,
		const s2f* B, const s2f* C, const s2f* D, float minLength )
{
	list< s2f >* points = new list< s2f >;
	Closure c;
	c.points = points;
	c.random = rndm;
	c.min_length = minLength;

	points->push_back( *A );
	subdivide( A, B, C, D, c );
	points->push_back( *C );
	return points;
}
