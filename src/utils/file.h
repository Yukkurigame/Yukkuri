/*
 * file.h
 *
 *  Created on: 09.04.2013
 */

#ifndef YUTILS_FILE_H_
#define YUTILS_FILE_H_

#include <stdio.h>

namespace YFILE
{

	/* Read from file 4 bytes big endian and return it as integer
	 * File will be read from current offset
	 * returns long int result
	 */
	long int read_libe( FILE* file );

	/* Write integer to file as 4 bytes big endian
	 * File will be writen from current offset
	 */
	void write_libe( FILE* file, long int src );



}



#endif /* YUTILS_FILE_H_ */
