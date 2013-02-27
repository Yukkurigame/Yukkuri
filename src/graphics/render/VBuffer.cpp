/*
 * VBuffer.cpp
 *
 *  Created on: 21.01.2013
 *
 */

#include "graphics/render/VBuffer.h"
#include "graphics/render/Textures.h"
#include "graphics/utils/VBOArray.h"
#include "graphics/utils/gl_uniforms.h"
#include "graphics/Camera.h"
#include <cstring>



/* TODO:
 *
 * GL_ELEMENT_ARRAY_BUFFER
 * glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DR
 *
 */

namespace VBuffer {

	struct BufferHandler {
		UINT id;
		GLuint handle;
		UINT size;
	};

	UINT buffers_count = 0;
	BufferHandler* buffers = NULL;
}


void VBuffer::create( UINT* buffer_id )
{
	UINT new_count = buffers_count + 1;
	buffers = (BufferHandler*)realloc( buffers, (UINT)sizeof(BufferHandler)*new_count );
	BufferHandler* hdl = &buffers[buffers_count];
	glGenBuffers( 1, &hdl->handle );
	hdl->id = buffers_count;
	hdl->size = 0;
	buffers_count = new_count;
	*buffer_id = hdl->id;
}

/*	This function removes VAO
 *
 */
void VBuffer::free_buffer( UINT* buffer_id )
{
	BufferHandler* hdl = &buffers[*buffer_id];
	hdl->id = 0;
	glDeleteBuffers( 1, &hdl->handle );
	*buffer_id = 0;
}



/*	This function binds VAO and set up pointers positions.
 *  handle - VAO id.
 */
void VBuffer::bind( UINT buffer_id )
{
	BufferHandler* hdl = &buffers[buffer_id];

	glBindBuffer( GL_ARRAY_BUFFER, hdl->handle );

	// Set up VBO strides & offsets
	int vertex_size =  sizeof(VertexV2FT2FC4UI);

	// Enable attrib
	glEnableVertexAttribArray( gllPosition );
	glEnableVertexAttribArray( gllTexCoord );
	glEnableVertexAttribArray( gllColor );
	glVertexAttribPointer( gllPosition, 3, GL_FLOAT, GL_FALSE, vertex_size, 0 );
	glVertexAttribPointer( gllTexCoord, 2, GL_FLOAT, GL_FALSE, vertex_size, BUFFER_OFFSET(sizeof(s3f)) );
	glVertexAttribPointer( gllColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, vertex_size, BUFFER_OFFSET(sizeof(s3f) + sizeof(s2f)) );

}


/*	This function ubinds VAO. */
void VBuffer::unbind()
{
	glDisableVertexAttribArray( gllPosition );
	glDisableVertexAttribArray( gllTexCoord );
	glDisableVertexAttribArray( gllColor );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


/*	This function pass verticles array data to VAO
 *  VAO must be binded perviously.
 */
void VBuffer::fill( UINT buffer_id )
{
	BufferHandler* hdl = &buffers[buffer_id];
	VertexV2FT2FC4UI* head = VBOArray::head();
	UINT size = VBOArray::size();
	if( size != hdl->size ){
		glBufferData( GL_ARRAY_BUFFER, size, head, GL_STREAM_DRAW );
		hdl->size = size;
	}else{
		glBufferSubData( GL_ARRAY_BUFFER, 0, size, head );
	}
}


/*	This function is just call of bind and fill together.
 *  handle - VAO id.
 */
void VBuffer::setup( UINT buffer_id )
{
	bind( buffer_id );
	fill( buffer_id );
}

inline void apply_material( UINT matid, int pass )
{
	const GLMaterial* mat = GLMaterialManager::get_pointer( matid );
	if( !mat )
		return;
	int shader = mat->programs[pass];
	UniformHandlers* uniforms = mat->uniforms[pass];
	glUseProgram( shader );
	if( !shader )
		return;

	int samplers_index = -1;
	if( uniforms ){
		for( unsigned int index = 0; index < uniforms->count; ++index ){
			UniformHandler* uniform = &uniforms->handlers[index];
			if( uniform->type == GL_SAMPLER_2D )
				UniformsManager::pass_data( uniform->index, &(++samplers_index) );
			UniformsManager::send_data( uniform->location, uniform->index );
		}
	}
}


/*	This function draws vertex array object
 * 	pass - number of pass to draw
 * 	vbostructure - linked list of vao description
 */
void VBuffer::draw( int pass, list<VBOStructureHandle*>* handler )
{
	listElement<VBOStructureHandle*>* handler_element = handler->head;

	int flags = 0; // TEXTURE_2D - 1

	//glEnable(GL_TEXTURE_2D);
	while(handler_element != NULL){
		VBOStructureHandle* vbostructure = handler_element->data;
		Textures::apply( &vbostructure->textures, flags );
		apply_material( vbostructure->material, pass );
		glDrawElements( vbostructure->method, vbostructure->count, GL_UNSIGNED_INT, vbostructure->indexes );
		handler_element = handler_element->next;
	}
	glUseProgram( 0 );

	if( flags & 1 )
		glDisable(GL_TEXTURE_2D);
}


/*	This function make vao array from single sprite
 *	sprite - pointer of sprite
 *	handler - pointer to handler list
 */
void VBuffer::prepare_handler( Sprite* sprite, list<VBOStructureHandle*>* handler )
{
	if( sprite == NULL || !sprite->isVisible() )
		return;

	VBOStructureHandle* v = NULL;
	if( handler->tail )
		v = handler->tail->data;

	if( !v || v->method != GL_TRIANGLES ||
		v->material != sprite->material ||
		!v->textures.cmp( &sprite->textures ) ){
		v = new VBOStructureHandle( sprite->brush.method, &sprite->textures, sprite->material );
		handler->push_back( v );
	}

	sprite->brush.update_points();
	v->set_indexes( sprite->brush.indices_list, sprite->brush.indices_count );
}


/*	This function make vao array from sprite array
 *	sprites - array of sprites
 *	handler - pointer to handler list
 */
void VBuffer::prepare_handler( list< Sprite* >* sprites, list<VBOStructureHandle*>* handler )
{
	listElement< Sprite* >* sprites_element = sprites->head;
	while( sprites_element != NULL ){
		prepare_handler( sprites_element->data, handler );
		sprites_element = sprites_element->next;
	}
}

/*	This function removes all elements from vao handler
 *	handler - Pointer to handlers list
 */
void VBuffer::free_handler( list< VBOStructureHandle* >* handler )
{
	listElement< VBOStructureHandle* >* handler_element = handler->head;
	while( handler_element != NULL ){
		delete handler_element->data;
		handler_element = handler_element->next;
	}
	handler->clear();
}

