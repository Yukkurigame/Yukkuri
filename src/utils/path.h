/*
 * path.h
 *
 *  Created on: 23.07.2012
 *
 */

#ifndef PATH_H_
#define PATH_H_

namespace Path {

	const char* app();

	int change_dir( const char* path );
	void init( );

	char* join( const char* first, const char* sec );
	int split( char*** dst, const char* src );
	int mkdir_recursive( const char* path, unsigned int perms );
}

#endif /* PATH_H_ */
