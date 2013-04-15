/*
 * Histogram.h
 *
 *  Created on: 20.03.2013
 */

#ifndef MAPGEN_HISTOGRAM_H_
#define MAPGEN_HISTOGRAM_H_

#include "map/generator/graph/Center.h"
#include "map/generator/Constants.h"
#include <cmath>

inline UINT interpolateColor( unsigned int c0, unsigned int c1, double f )
{
	unsigned int r = ( 1 - f ) * ( c0 >> 16 ) + f * ( c1 >> 16 );
	unsigned int g = ( 1 - f ) * ( ( c0 >> 8 ) & 0xff ) + f * ( ( c1 >> 8 ) & 0xff );
	unsigned int b = ( 1 - f ) * ( c0 & 0xff ) + f * ( c1 & 0xff );
	if( r > 255 )
		r = 255;
	if( g > 255 )
		g = 255;
	if( b > 255 )
		b = 255;
	return ( r << 16 ) | ( g << 8 ) | b;
}

inline int landTypeBucket( Center* p )
{
	if( p->ocean )
		return 1;
	else if( p->coast )
		return 2;
	else if( p->water )
		return 3;
	return 4;
}

inline UINT landTypeColor( int bucket ) {
	if( bucket == 1 )
		return gcOCEAN;
	else if( bucket == 2 )
		return gcBEACH;
	else if( bucket == 3 )
		return gcLAKE;
	return gcTEMPERATE_DECIDUOUS_FOREST;
}

inline int elevationBucket( Center* p )
{
	if( p->ocean )
		return -1;
	else
		return floor( p->elevation * 10 );
}

inline UINT elevationColor( int bucket )
{
	return interpolateColor( gcTEMPERATE_DECIDUOUS_FOREST, gcGRASSLAND, bucket*0.1);
}

inline int moistureBucket( Center* p )
{
	if( p->water )
		return -1;
	else
		return floor( p->moisture * 10 );
}

inline UINT moistureColor( int bucket )
{
	return interpolateColor( gcBEACH, gcRIVER, bucket * 0.1 );
}

inline int biomeBucket( Center* p )
{
	return p->biome;
}

inline UINT biomeColor( int bucket )
{
	return BiomesColors[bucket];
}

typedef int (*bucketFn)( Center* );
typedef UINT (*colorFn)( int );

#endif /* MAPGEN_HISTOGRAM_H_ */
