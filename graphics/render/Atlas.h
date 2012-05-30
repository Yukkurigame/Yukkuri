/*
 * Atlas.h
 *
 *  Created on: 28.05.2012
 *
 */

#ifndef ATLAS_H_
#define ATLAS_H_

#include <string>
#include "graphics/GraphicsTypes.h"

namespace TextureAtlas
{
	void init( );
	void clean( );

	int getAtlasMax( );

	void addTexture( std::string name );
	void addTexture( std::string id, Texture* tex, int width, int height,
						int cols, int rows, int ax, int ay );
	void addTexture( std::string id, TextureInfo* tex, int picture );

	bool buildMap( int& width, int& height );
	bool buildRelativeMap( float width, float height );
	bool build( GLuint* ahandle, int width, int height );

	bool create( GLuint* ahandle, int& width, int& height );

}


#endif /* ATLAS_H_ */
