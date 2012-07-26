/*
 * misc.cpp
 *
 *  Created on: 23.07.2012
 *
 */

#include "misc.h"
#include "Define.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace {
	char* path_app;
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


const char* Paths::app( )
{
	return path_app;
}



int Paths::change_dir( const char* path )
{
#ifdef WIN32
	return _chdir(path);
#else
	return chdir(path);
#endif // WIN32
}


void Paths::init(  )
{
	path_app = new char[ MAX_PATH ];

#ifdef WIN32
	// application
	GetModuleFileName( GetModuleHandle(0), path_app, MAX_PATH );
	path_app = dirname( path_app );
	change_dir( path_app );

	GetCurrentDirectory( MAX_PATH, path_app );
	strcat( path_app, "\\" );

#elif defined(__APPLE__)
	uint32_t size = sizeof(path_app);
	if (_NSGetExecutablePath(path_app, &size) == 0){
		printf( "\nexecutable path is %s\n", path_app );
		path_app = dirname( path_app );
		path_app[ strlen(path_app) - 1 ] = 0;
		strcat( path_app, "/" );
		printf( "working path will be %s\n", path_app );
		change_dir( path_app );
	}else{
		printf( "buffer too small; need size %u\n", size );
	}

#elif defined(__linux__)
	// TODO: Метод  только для linux. Поменять для других *nix-систем.
	ssize_t count = readlink("/proc/self/exe", path_app, MAX_PATH );
	if( count > 0 ){
		path_app[count] = 0;
		path_app = dirname( path_app );
		change_dir( path_app );
		strcat( path_app, "/" );
		printf( "path_app: %s\n", path_app );
	}else{
		printf( "Error getting application path" );
		exit( EXIT_FAILURE );
	}
#elif defined(__FreeBSD__)
	memset( path_app, 0, MAX_PATH );
	bool res = readlink( "/proc/curproc/file", path_app, MAX_PATH ) > 0;

	if( !res ){
		puts( "readlink failed. Trying to use sysctl" );
		int mib[4];
		mib[0] = CTL_KERN;
		mib[1] = KERN_PROC;
		mib[2] = KERN_PROC_PATHNAME;
		mib[3] = -1;
		size_t cb = MAX_PATH;
		res = sysctl(mib, 4, path_app, &cb, NULL, 0) == 0;
	}

	if( res ){
		printf( "executable path: %s\n", path_app );
		path_app = dirname( path_app );
		strcat( path_app, "/" );
		change_dir(path_app);
		printf( "path_app: %s\n", path_app );
	}else{
		printf( "Error getting application path" );
		exit(EXIT_FAILURE);
	}
#else
	#error Unsupported platform.
#endif // WIN32

}

