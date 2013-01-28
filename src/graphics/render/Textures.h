/*
 * Textures.h
 *
 *  Created on: 26.01.2013
 */

#ifndef TEXTURES_H_
#define TEXTURES_H_

#include "types.h"
#include "graphics/GraphicsTypes.h"
#include "utils/list.h"


namespace Textures {

	void clean( );

	UINT get_by_name( const char* id );
	TextureInfo* get_pointer( UINT number );

	UINT get_active( UINT id );

	// Textures storage

	bool load( );

	UINT push( TextureProxy*, GLuint atlas, GLuint normals );
	void push( list< TextureProxy* >& tarray, GLuint atlas, GLuint normals );

}



#endif /* TEXTURES_H_ */
