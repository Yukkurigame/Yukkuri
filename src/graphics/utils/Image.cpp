/*
 * Image.cpp
 *
 *  Created on: 28.01.2013
 *
 */


#include "graphics/utils/Image.h"
#include "SOIL.h"


unsigned int Image::load( const char* name, int* width, int* height, bool force )
{
	unsigned int img = 0;
	char* path = (char*)malloc( (unsigned int)conf.imagePath.size( ) + strlen( name ) + 1 );
	sprintf( path, "%s%s", conf.imagePath.c_str(), name );
	img = open( path, width, height );
	if( !img && force ){
		//Not loaded. Try again with default path if path is not default already.
		if( strcmp( name, conf.defaultImage.c_str() ) ){
			sprintf( path, "%s", conf.defaultImage.c_str() );
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
