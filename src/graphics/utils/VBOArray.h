/*
 * VBOArray.h
 *
 *  Created on: 05.12.2012
 */

#ifndef VBOARRAY_H_
#define VBOARRAY_H_

#include "graphics/GraphicsTypes.h"


namespace VBOArray {

	int getSpace( unsigned int size );
	void freeSpace( unsigned int index, unsigned int count );
	VertexV2FT2FC4UI* pointer( unsigned int index );
	inline VertexV2FT2FC4UI* head( ){ return pointer(0); }
	int count( );
	inline int size( ){ return count() * sizeof(VertexV2FT2FC4UI); }

}


#endif /* VBOARRAY_H_ */
