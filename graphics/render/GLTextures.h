/*
 * GLTextures.h
 *
 *  Created on: 17.07.2012
 *
 */

#ifndef GLTEXTURES_H_
#define GLTEXTURES_H_

#include "graphics/GraphicsTypes.h"
#include <string>


namespace GLTextures
{

	void add( std::string name, Texture* texture );
	void free( Texture* tex );

	Texture* get( std::string name );
	Texture* load( std::string name );

	void clearCache( );

}


#endif /* GLTEXTURES_H_ */
