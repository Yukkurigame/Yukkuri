/*
 * PMPRNG.h
 *
 * Copyright (c) 2009 Michael Baczynski, http://www.polygonal.de
 * Copyright (c) 2013 Sir Anthony anthony [at] adsorbtion.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * Implementation of the Park Miller (1988) "minimal standard" linear
 * congruential pseudo-random number generator.
 *
 * For a full explanation visit: http://www.firstpr.com.au/dsp/rand31/
 *
 * The generator uses a modulus constant (m) of 2^31 - 1 which is a
 * Mersenne Prime number and a full-period-multiplier of 16807.
 * Output is a 31 bit unsigned integer. The range of values output is
 * 1 to 2,147,483,646 (2^31-1) and the seed must be in this range too.
 *
 * David G. Carta's optimisation which needs only 32 bit integer math,
 * and no division is actually *slower* in flash (both AS2 & AS3) so
 * it's better to use the double-precision floating point version.
 *
 * @author Michael Baczynski, www.polygonal.de
 * C++ adaptation - Sir Anthony
 */


#ifndef PMPRNG_H_
#define PMPRNG_H_

class PM_PRNG {
public:
	/**
	 * set seed with a 31 bit unsigned integer
	 * between 1 and 0X7FFFFFFE inclusive. don't use 0!
	 */
	unsigned int seed;

	PM_PRNG( );
	virtual ~PM_PRNG( );

	/**
	 * provides the next pseudorandom number
	 * as an unsigned integer (31 bits)
	 */
	unsigned int nextInt( );

	/**
	 * provides the next pseudorandom number
	 * as a float between nearly 0 and nearly 1.0.
	 */
	double nextDouble( );

	/**
	 * provides the next pseudorandom number
	 * as an unsigned integer (31 bits) betweeen
	 * a given range.
	 */
	unsigned int nextIntRange( double min, double max );

	/**
	 * provides the next pseudorandom number
	 * as a float between a given range.
	 */
	double nextDoubleRange( double min, double max );


	/**
	 * generator:
	 * new-value = (old-value * 16807) mod (2^31 - 1)
	 */
	unsigned int gen( );

};

#endif /* PMPRNG_H_ */
