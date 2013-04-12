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
	/* Dump bson data as char* to write to file
	 * returns length of data dumped in string
	 */
	int dump_bson( char*& output, bson* b );


	/* Internal template to construct data from bson
	 * Dump bson data to various types (unpack it)
	 * returns count of successfully processed fields
	 */
	template< typename T >
	int dump_bson( T, bson* );

	/* Internal template to construct bson from data
	 * returns positive number on success
	 */
	template< typename T >
	int construct_bson( bson*, T );

	/* Internal template to construct data from bson iterator
	 * returns positive number on success
	 */
	template< typename T >
	int construct_data( bson_iterator* iter, T& data );

	/* External interface. This function must be used for all
	 * data-to-bson_string operations
	 * output - pointer to new data array (will be allocated)
	 * length - result array size
	 * data - container
	 * returns positive number on success
	 */
	template< typename T >
	int pack( char*& output, int& length, T data )
	{
		bson b;
		int ret = 0;
		bson_init( &b );
		ret = construct_bson( &b, data );
		bson_finish( &b );
		if( ret )
			length = dump_bson( output, &b );
		bson_destroy( &b );
		return ret;
	}

	/* External interface. This function must be used for all
	 * bson_string-to-data operations
	 * input - bson array (will be freed) if owned is true
	 * data - container to fill
	 * owned - flag for bson initializer to free data after processing
	 * returns positive number on success
	 */
	template< typename T >
	int unpack( char* input, T data, bool owned = true )
	{
		bson b;
		int ret = 0;
		bson_init_finished_data( &b, input, owned );
		ret = dump_bson( data, &b );
		bson_destroy( &b );
		return ret;
	}

}


#endif /* YBSON_H_ */
