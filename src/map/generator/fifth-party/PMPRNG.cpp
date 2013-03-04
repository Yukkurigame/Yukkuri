/*
 * PMPRNG.cpp
 *
 *  Created on: 02.03.2013
 */

#include "PMPRNG.h"
#include <math.h>

PM_PRNG::PM_PRNG( )
{
	seed = 1;
}

PM_PRNG::~PM_PRNG( )
{
	// TODO Auto-generated destructor stub
}

unsigned int PM_PRNG::nextInt( )
{
	return gen();
}


double PM_PRNG::nextDouble( )
{
	return ((double)gen() / 2147483647.0);
}


unsigned int PM_PRNG::nextIntRange( double min, double max )
{
	min -= .4999;
	max += .4999;
	return round( min + ((max - min) * nextDouble()) );
}


double PM_PRNG::nextDoubleRange( double min, double max )
{
	return min + ((max - min) * nextDouble());
}


unsigned int PM_PRNG::gen( )
{

	//integer version 1, for max int 2^46 - 1 or larger.
	return seed = (seed * 16807) % 2147483647;

	/**
	 * integer version 2, for max int 2^31 - 1 (slowest)
	 */
	//int test = 16807 * (seed % 127773 >> 0) - 2836 * (seed / 127773 >> 0);
	//return seed = (test > 0 ? test : test + 2147483647);

	/**
	 * david g. carta's optimisation is 15% slower than integer version 1
	 */
	//unsigned int hi = 16807 * (seed >> 16);
	//unsigned int lo = 16807 * (seed & 0xFFFF) + ((hi & 0x7FFF) << 16) + (hi >> 15);
	//return seed = (lo > 0x7FFFFFFF ? lo - 0x7FFFFFFF : lo);
}

