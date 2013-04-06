/*
 * misc.cpp
 *
 *  Created on: 23.07.2012
 *
 */

#include "utils/path.h"
#include "Define.h"
#include "debug.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <cerrno>


namespace Path {
	char* path_app;


	/* determine if "ch" is a separator character */
	int is_sep( char ch ){
	#ifdef ALTSEP
		return ch == SEP || ch == ALTSEP;
	#else
		return ch == SEP;
	#endif
	}

}



#ifndef WIN32
	#include <libgen.h>
	#include <unistd.h>
#else
	#include <windows.h>
	#include <direct.h>
	char* dirname(char* path){
		_splitpath(path, NULL, path, NULL, NULL);
		return buf;
	}
#endif


const char* Path::app( )
{
	return path_app;
}



int Path::change_dir( const char* path )
{
#ifdef WIN32
	return _chdir(path);
#else
	return chdir(path);
#endif // WIN32
}


void Path::init(  )
{
	path_app = new char[ MAXPATHLEN ];

#ifdef WIN32
	// application
	GetModuleFileName( GetModuleHandle(0), path_app, MAXPATHLEN );
	path_app = dirname( path_app );
	change_dir( path_app );

	GetCurrentDirectory( MAXPATHLEN, path_app );
	strcat( path_app, "\\" );

#elif defined(__APPLE__)
	uint32_t size = sizeof(path_app);
	if (_NSGetExecutablePath(path_app, &size) == 0){
		Debug( Debug::OS, "Executable path is %s.\n", path_app );
		path_app = dirname( path_app );
		path_app[ strlen(path_app) - 1 ] = 0;
		strcat( path_app, "/" );
		Debug( Debug::OS, "Working path will be %s.\n", path_app );
		change_dir( path_app );
	}else{
		Debug( Debug::OS, "Buffer is too small; need size %u\n", size );
	}

#elif defined(__linux__)
	// TODO: Метод  только для linux. Поменять для других *nix-систем.
	ssize_t count = readlink("/proc/self/exe", path_app, MAXPATHLEN );
	if( count > 0 ){
		path_app[count] = 0;
		path_app = dirname( path_app );
		change_dir( path_app );
		strcat( path_app, "/" );
		Debug::debug( Debug::OS, "path_app: %s\n", path_app );
	}else{
		Debug::debug( Debug::OS, "Error getting application path.\n" );
		exit( EXIT_FAILURE );
	}
#elif defined(__FreeBSD__)
	memset( path_app, 0, MAXPATHLEN );
	bool res = readlink( "/proc/curproc/file", path_app, MAXPATHLEN ) > 0;

	if( !res ){
		puts( "readlink failed. Trying to use sysctl" );
		int mib[4];
		mib[0] = CTL_KERN;
		mib[1] = KERN_PROC;
		mib[2] = KERN_PROC_PATHNAME;
		mib[3] = -1;
		size_t cb = MAXPATHLEN;
		res = sysctl(mib, 4, path_app, &cb, NULL, 0) == 0;
	}

	if( res ){
		Debug( Debug::OS, "Executable path is %s.\n", path_app );
		path_app = dirname( path_app );
		strcat( path_app, "/" );
		change_dir(path_app);
		Debug( Debug::OS, "path_app: %s.\n", path_app );
	}else{
		Debug::debug( Debug::OS, "Error getting application path.\n" );
		exit( EXIT_FAILURE );
	}
#else
	#error Unsupported platform.
#endif // WIN32

}


/*	Join two strings into one use system separator
 *  join("a", "b") -> "a/b" or "a\b"
 */
char* Path::join( const char* first, const char* sec )
{
	size_t f, n, k;
	f = strlen(first);
	k = strlen(sec);

	char* ret = (char*)malloc( sizeof(char) * ( f + k + 3 ) );
	strncpy( ret, first, f );
	ret[f] = '\0';

	if( is_sep(sec[0]) ){
		n = 0;
	}else{
		n = strlen( ret );
		if( n > 0 && !is_sep(ret[n-1]) && n < MAXPATHLEN )
			ret[n++] = SEP;
	}

	if( n > MAXPATHLEN ){
		Debug::debug( Debug::OS, "Source path longer than maximum path length.\n" );
		free(ret);
		return NULL;
	}

	if( n + k > MAXPATHLEN )
		k = MAXPATHLEN - n;

	strncpy( ret + n, sec, k );
	ret[n+k] = '\0';

	return ret;
}

/* Split path into array of strings tokenized by sep
 * "a/b" -> ["a", "b"]
 */
int Path::split( char*** dst, const char* src )
{
	// TODO: use token instead of is_sep
	int count = 0;
	int srcend = strlen(src);
	int index = srcend;
	int part_end = srcend;
	char** reverse_names = NULL;
	while( index > 0 ){
		while( !is_sep(src[index]) && index > 0 )
			--index;
		int part_length = part_end - index;
		// In separator
		if( !part_length ){
			--index;
			--part_end;
			continue;
		}

		count++;
		reverse_names = (char**)realloc( reverse_names, sizeof(char*) * count );
		reverse_names[count-1] = (char*)malloc( sizeof(char) * part_length );
		strncpy( reverse_names[count-1], &src[index+1], part_length );
		reverse_names[count-1][part_length] = '\0';
		part_end = index;
	}

	if( count ){
		*dst = (char**)malloc( sizeof(char*) * count );
		for( int i = 0; i < count; ++i ){
			(*dst)[(count - 1) - i] = reverse_names[i];
		}
		free( reverse_names );
	}

	return count;
}

/* Makes directory recursivly
 * Returns zero or ERRNO value on error
 */
int Path::mkdir_recursive( const char* path, unsigned int perms )
{
	int ret = 0;
	char** path_components = NULL;
	int components_length = split( &path_components, path );
	char* tmp_path = (char*) malloc( sizeof(char*) * 2 );
	tmp_path[0] = '\0';

	for( int i = 0; i < components_length; ++i ){
		if( ret >= 0 ){
			char* npath = join( tmp_path, path_components[i] );
			struct stat s;
			if( stat( npath, &s ) < 0 ){
				// Some error
				if( errno != ENOENT )
					ret = errno;
				else
					mkdir( npath, perms );
			}else if( !S_ISDIR(s.st_mode)){
				// Exists and not a directory.
				ret = ENOTDIR;
			}
			free( tmp_path );
			tmp_path = npath;
		}
		free( path_components[i] );
	}

	if( components_length )
		free(path_components);
	free( tmp_path );

	return ret;
}
