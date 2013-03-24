/*
 * misc.h
 *
 *  Created on: 14.02.2013
 */

#ifndef MISC_H_
#define MISC_H_

#include "basic_types.h"
#include <cmath>

inline int next_p2( int a )
{
	int rval=2;
	while( rval < a ) rval <<= 1;
	return rval;
}

inline s2f interpolate( const s2f& first, const s2f& second, float delta )
{
	return s2f( first.x + delta * ( second.x - first.x ),
			first.y + delta * ( second.y - first.y ) );
}

inline double length( const s2f& v )
{
	return sqrt( v.x * v.x + v.y * v.y );
}

inline s2f normalize( const s2f& f )
{
	double l = length( f );
	return s2f( f.x / l, f.y / l );
}

inline double max( double first, double second )
{
	return ( first > second ) ? first : second;
}

#endif /* MISC_H_ */
