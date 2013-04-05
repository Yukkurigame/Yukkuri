/*
 * rand31.h
 *
 *  Created on: 25.03.2013
 */

#ifndef RAND31_H_
#define RAND31_H_


// rand31-park-miller-carta.cc
// ---------------------------
//
// Robin Whittle  rw@firstpr.com.au    2005 September 21
//
// For more information and updates: http://www.firstpr.com.au/dsp/rand31/
//
// This code contains two separate implementations of the 1988 Park Miller
// "minimal standard"  31 bit PRNG (Pseudo-Random Number Generator):
//


class rand31
{
public:
    rand31() : seed31(1) { }

	void seed( long unsigned int );
	long unsigned int nextInt( void );
	long unsigned int nextInt( double min, double max );
	double nextDouble( void );
	double nextDouble( double min, double max );

private:
    long unsigned int seed31;		// The sole item of state - a 32 bit integer.
    long unsigned int nextrand();
};


#endif /* RAND31 */
