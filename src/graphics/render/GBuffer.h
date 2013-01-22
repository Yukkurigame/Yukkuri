/*
 * GBuffer.h
 *
 *  Created on: 11.01.2013
 *
 */

#ifndef GBUFFER_H_
#define GBUFFER_H_

#include "graphics/GraphicsTypes.h"


namespace GBuffer
{
	enum texture_type {
		gbufTPosition,
		gbufTDiffuse,
		gbufTNormal,
		gnufTUi,
		gbufLast
	};

	bool init( );
	void clean( );

	void render();
}



#endif /* GBUFFER_H_ */
