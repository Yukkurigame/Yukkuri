/*
 * VBuffer.h
 *
 *  Created on: 21.01.2013
 *
 */

#ifndef VBUFFER_H_
#define VBUFFER_H_

#include "graphics/GraphicsTypes.h"
#include "graphics/sprite/Sprite.h"


namespace VBuffer {

	// VAO
	void create( GLuint* handle );
	void bind( GLuint handle );
	void unbind(  );
	void fill( );
	void setup( GLuint handle );
	void draw( int pass, list<VBOStructureHandle*>* /*VBOStructureHandle* vbostructure*/ );

	// Structures
	void prepare_handler( Sprite* sprite, list<VBOStructureHandle*>* );
	void prepare_handler( list< Sprite* >* sprites, list<VBOStructureHandle*>* );
	//VBOStructureHandle* prepare_handler( int pass, Sprite* sprite );
	//VBOStructureHandle* prepare_handler( int pass, list< Sprite* >* sprites );
	//VBOStructureHandle* prepare_handler( Sprite* sprites, unsigned int scount );
	void free_handler( list<VBOStructureHandle*>* );

}



#endif /* VBUFFER_H_ */