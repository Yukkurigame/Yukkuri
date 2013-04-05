/*
 * bson.h
 *
 *  Created on: 05.04.2013
 */

// Pack/unpack bson data

#ifndef YBSON_H_
#define YBSON_H_

namespace BSON
{
	template< typename T >
	int pack( char*&, int& length, T );
}


#endif /* YBSON_H_ */
