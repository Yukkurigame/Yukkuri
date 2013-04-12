/*
 * file.cpp
 *
 *  Created on: 09.04.2013
 */


#include "utils/file.h"
#include "debug.h"
#include <climits>


long int YFILE::read_libe( FILE* file )
{
	long int bits = 0;
	for( int i = 0; i < 4; i++ )
		bits = ( bits << 8 ) + fgetc( file );

	return bits < (long)0x80000000 ? bits : -1 - (long)(bits ^ 0xffffffff);

}

void YFILE::write_libe( FILE* file, long int src )
{
	if( src > LONG_MAX || src < LONG_MIN )
		Debug::debug( Debug::MAIN, "Attempt to write very big number as long" );

	for( int i = 0; i < 4; ++i )
		fputc( ( src >> ( 8 * ( 3 - i ) ) ) & 0xff, file );
}
