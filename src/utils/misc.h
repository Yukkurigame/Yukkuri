/*
 * misc.h
 *
 *  Created on: 14.02.2013
 */

#ifndef MISC_H_
#define MISC_H_

inline int next_p2( int a )
{
	int rval=2;
	while( rval < a ) rval <<= 1;
	return rval;
}


#endif /* MISC_H_ */
