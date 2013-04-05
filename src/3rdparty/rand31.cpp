/*
 * rand31.cpp
 *
 *  Created on: 25.03.2013
 */

#include "3rdparty/rand31.h"
#include <cmath>

#define CONSTA 16807
#define CONSTM 2147483647.0


/*
 * Set the seed from a long unsigned
 * integer.  If zero is used, then
 * the seed will be set to 1.
 */
void rand31::seed( long unsigned int seedin )
{
	if( seedin == 0 )
		seedin = 1;
	seed31 = seedin;
}

/*
 * Return next pseudo-random value as
 * a long unsigned integer.
 */
long unsigned int rand31::nextInt( void )
{
	return nextrand();
}

/*
 * Return next pseudo-random value as
 * a long unsigned integer in specified range.
 */
long unsigned int rand31::nextInt( double min, double max )
{
	min -= .4999;
	max += .4999;
	return round( min + ((max - min) * nextDouble()) );
}

/*
 * Return next pseudo-random value as
 * a floating point value.
 */
double rand31::nextDouble( void )
{
	return ( (double)nextrand() / CONSTM );
}

/*
 * Return next pseudo-random value as
 * a floating point value in specified range.
 */
double rand31::nextDouble( double min, double max )
{
	return min + ((max - min) * nextDouble());
}

/*
 * Generate new integer
 */
long unsigned int rand31::nextrand( )
{
	//integer version 1, for max int 2^46 - 1 or larger.
	//return seed = (seed * CONSTA) % 2147483647;

	long unsigned int hi, lo;
	hi = CONSTA * ( seed31 >> 16 );
	lo = CONSTA * ( seed31 & 0xFFFF ) + ( ( hi & 0x7FFF ) << 16 ) + ( hi >> 15 );
	return ( seed31 = (long)( lo > 0x7FFFFFFF ? lo - 0x7FFFFFFF : lo ) );
}
