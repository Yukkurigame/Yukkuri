/*
 * VBuffer.cpp
 *
 *  Created on: 21.01.2013
 *
 */

#include "graphics/render/VBuffer.h"
#include "graphics/render/Textures.h"
#include "graphics/utils/gl_uniforms.h"
#include "graphics/Camera.h"
#include <cstring>



/* TODO:
 *
 * GL_ELEMENT_ARRAY_BUFFER
 * glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DR
 *
 */

namespace {
	int current_shader = -2;
}


namespace VBuffer {

	UINT buffers_count = 0;
	VBufferHandler* buffers = NULL;
	UINT binded = 0;

}


void VBuffer::create( UINT* buffer_id )
{
	UINT new_count = buffers_count + 1;
	buffers = (VBufferHandler*)realloc( buffers, (UINT)sizeof(VBufferHandler)*new_count );
	VBufferHandler* hdl = &buffers[buffers_count];
	hdl->array.init();
	glGenBuffers( 1, &hdl->handle );
	*buffer_id = buffers_count;
	buffers_count = new_count;
	hdl->id = new_count; // buffers_count + 1; first id - 1
	hdl->size = 0;
}

/*	This function removes VAO
 *
 */
void VBuffer::free_buffer( UINT* buffer_id )
{
	VBufferHandler* hdl = &buffers[*buffer_id];
	hdl->id = 0;
	hdl->array.clear();
	glDeleteBuffers( 1, &hdl->handle );
	*buffer_id = 0;
}


/*	This function binds VAO and set up pointers positions.
 *  handle - VAO id.
 */
void VBuffer::bind( UINT buffer_id )
{
	VBufferHandler* hdl = &buffers[buffer_id];
	if( binded == hdl->id )
		return;

	glBindBuffer( GL_ARRAY_BUFFER, hdl->handle );
	binded = hdl->id;

	enable_attrib( );
}

/*	This function enables attrib array and setup vbo pointers */
void VBuffer::enable_attrib( )
{
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

/*	This function disables attrib array. */
void VBuffer::disable_attrib( )
{
	glDisableVertexAttribArray( gllPosition );
	glDisableVertexAttribArray( gllTexCoord );
	glDisableVertexAttribArray( gllColor );
}


/*	This function ubinds VAO. */
void VBuffer::unbind()
{

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	binded = 0;
}

/*	This function updates subdata in VBO
 *	buffer_id - number of VBO
 *	index - first index of data block
 *	count - count of points in data block
 */
void VBuffer::pass_subdata( UINT buffer_id, int index, int count )
{
	VBufferHandler* hdl = &buffers[buffer_id];
	if( binded != hdl->id ){
		glBindBuffer( GL_ARRAY_BUFFER, hdl->handle );
		binded = hdl->id;
	}
	UINT total_size = hdl->array.size();
	if( total_size != hdl->size ){
		glBufferData( GL_ARRAY_BUFFER, total_size, hdl->array.head(), GL_STREAM_DRAW );
		hdl->size = total_size;
		return;
	}
	UINT size = count * sizeof(VertexV2FT2FC4UI);
	UINT first = index * sizeof(VertexV2FT2FC4UI);
	glBufferSubData( GL_ARRAY_BUFFER, first, size, hdl->array.pointer(index) );

}


/*	This function returns handler to VBO
 *	buffer_id - id of VBO
 *	returns pointer to buffer handler.
 */
VBufferHandler* VBuffer::handler( UINT buffer_id )
{
	if( buffer_id < buffers_count ){
		VBufferHandler* hdl = &buffers[buffer_id];
		if( hdl->id )
			return hdl;
	}
	return NULL;
}


/*	This function returns pointer to VBO array data with given offset
 * 	buffer_id - number of VBO
 * 	index - data block offset
 */
VertexV2FT2FC4UI* VBuffer::array_pointer( UINT buffer_id, int index )
{
	VBufferHandler* hdl = handler(buffer_id);
	if(hdl)
		return hdl->array.pointer(index);
	return NULL;
}


/*	This function pass verticles array data to VAO
 *  VAO must be binded perviously.
 */
void VBuffer::fill( UINT buffer_id )
{
	VBufferHandler* hdl = &buffers[buffer_id];
	VertexV2FT2FC4UI* head = hdl->array.head();
	UINT size = hdl->array.size();
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
	//if( shader != current_shader ){
		glUseProgram( shader );
		//current_shader = shader;
	//}
	if( !shader )
		return;

	int samplers_index = -1;
	UniformHandlers* uniforms = mat->uniforms[pass];
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

	VBuffer::unbind();

	while(handler_element != NULL){
		VBOStructureHandle* vbostructure = handler_element->data;
		VBuffer::bind( vbostructure->vbo );
		Textures::apply( &vbostructure->textures, flags );
		apply_material( vbostructure->material, pass );
		glDrawElements( vbostructure->method, vbostructure->count, GL_UNSIGNED_INT, vbostructure->indexes );
		handler_element = handler_element->next;
	}
	VBuffer::disable_attrib();
	VBuffer::unbind( );
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
		v->material != sprite->material || v->vbo != sprite->brush.VBOHandler ||
		!v->textures.cmp( &sprite->textures ) ){
		v = new VBOStructureHandle( sprite->brush.method, sprite->brush.VBOHandler,
									&sprite->textures, sprite->material );
		handler->push_back( v );
	}

	if( sprite->brush.isUpdated() ){
		sprite->brush.update_points();
		pass_subdata( sprite->brush.VBOHandler, sprite->brush.point_index, sprite->brush.points_count );
	}

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

