/*
 * Image.cpp
 *
 *  Created on: 28.01.2013
 *
 */


#include "graphics/utils/Image.h"
#include "utils/path.h"
#include "SOIL.h"
#include "config.h"
#include "safestring.h"
#include "debug.h"
#include <cstdlib>

#include <sys/stat.h>


extern MainConfig conf;


namespace Image {
	int exists( char *name )
	{
		struct stat dummy;
		if( stat( name, &dummy ) == 0 )
			return 1;
		return 0;
	}

	void generate_name( char* name )
	{
		int iter = 0;
		do{
			snprintf( name, 50, "screenshot%03d.bmp", ++iter );
		}while( exists( name ) && iter < 1000 );
		if( exists( name ) ){
			name[0] = '\0';
		}
	}
}



unsigned int Image::load( const char* name, int* width, int* height, bool force )
{
	unsigned int img = 0;
	char* path = Path::join( conf.path.image.c_str(), name );
	img = open( path, width, height );
	if( !img && force ){
		//Not loaded. Try again with default path if path is not default already.
		if( strcmp( name, conf.default_image.c_str() ) ){
			sprintf( path, "%s", conf.default_image.c_str() );
			img = load( path, width, height );
		}
	}
	free(path);
	return img;
}


unsigned int Image::open( const char* filename, int* width, int* height )
{
	unsigned int tex_2d = SOIL_load_OGL_texture_sized( filename, SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT, width, height );
	if( !tex_2d ){
		const char* error = SOIL_last_result();
		int str_size = (25 + strlen(filename) + strlen( error )) * sizeof(char);
		char dbg[str_size];
		sprintf( dbg, "Could not load %s: %s.\n", filename, error );
		Debug::debug( Debug::GRAPHICS, dbg );
	}
	return tex_2d;
}


// TODO: SOIL save to png
bool Image::save( )
{
	char filename[52];
	generate_name( filename );
	if( filename[0] == '\0' ){
		Debug::debug( Debug::OS, "Can not get screenshot name. Too many screenshots in folder.\n" );
		return false;
	}

	return SOIL_save_screenshot( filename, SOIL_SAVE_TYPE_BMP, 0, 0,
			conf.video.windowWidth, conf.video.windowHeight );
}
