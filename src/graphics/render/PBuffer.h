/*
 * PBuffer.h
 *
 *  Created on: 08.02.2013
 */

#ifndef PBUFFER_H_
#define PBUFFER_H_

#include "graphics/GraphicsTypes.h"


namespace PBuffer
{

	bool copy( GLuint basetex, Texture* copysrc, const s2f& offset, int tx, int ty, int w, int h );
	void draw( GLuint tex, GLuint buf, int size, void* buffer );


}


#endif /* PBUFFER_H_ */
