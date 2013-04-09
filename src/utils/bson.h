/*
 * bson.h
 *
 *  Created on: 05.04.2013
 */

// Pack/unpack bson data

#ifndef YBSON_H_
#define YBSON_H_

#include "3rdparty/bson/bson.h"

namespace BSON
{
	int bson_dump( char*& output, bson* b );

	template< typename T >
	int construct_bson( bson*, T );

	template< typename T >
	int pack( char*& output, int& length, T data )
	{
		bson b;
		int ret = 0;
		bson_init( &b );
		ret = construct_bson( &b, data );
		bson_finish( &b );
		if( ret )
			length = bson_dump( output, &b );
		bson_destroy( &b );
		return ret;
	}

}


#endif /* YBSON_H_ */
