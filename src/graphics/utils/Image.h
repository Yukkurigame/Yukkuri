/*
 * Image.h
 *
 *  Created on: 28.01.2013
 *
 */

#ifndef IMAGE_H_
#define IMAGE_H_

namespace Image
{
	unsigned int load( const char*, int* width, int* height, bool force=true );
	unsigned int open( const char*, int* width, int* height );
	bool save( );
}

#endif /* IMAGE_H_ */
