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
#include "graphics/utils/VBOArray.h"


struct VBufferHandler {
	UINT id;
	GLuint handle;
	UINT size;
	VBOArray array;
};


namespace VBuffer {

	// VAO
	void create( UINT* buffer_id );
	void free_buffer( UINT* buffer_id );
	void bind( UINT buffer_id );
	void enable_attrib( );
	void disable_attrib( );
	void fill( UINT buffer_id );
	void unbind(  );

	void pass_subdata( UINT buffer_id, int index, int count );


	VBufferHandler* handler( UINT buffer_id );
	VertexV2FT2FC4UI* array_pointer( UINT buffer_id, int index );



	void setup( UINT buffer_id );
	void draw( int pass, list<VBOStructureHandle*>* /*VBOStructureHandle* vbostructure*/ );

	// Structures
	void prepare_handler( Sprite* sprite, list<VBOStructureHandle*>* );
	void prepare_handler( list< Sprite* >* sprites, list<VBOStructureHandle*>* );
	void free_handler( list<VBOStructureHandle*>* );

}



#endif /* VBUFFER_H_ */
