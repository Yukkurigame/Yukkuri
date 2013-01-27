/*
 * VBuffer.cpp
 *
 *  Created on: 21.01.2013
 *
 */

#include "graphics/render/VBuffer.h"
#include "graphics/utils/VBOArray.h"
#include "graphics/utils/gl_uniforms.h"
#include "graphics/Camera.h"
#include <cstring>


void VBuffer::create( GLuint* handle )
{
	glGenBuffers( 1, handle );
}


/*	This function binds VAO and set up pointers positions.
 *  handle - VAO id.
 */
void VBuffer::bind( GLuint handle )
{
	glBindBuffer( GL_ARRAY_BUFFER, handle );

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
void VBuffer::fill()
{
	VertexV2FT2FC4UI* head = VBOArray::head();
	int size = VBOArray::size();
	glBufferData( GL_ARRAY_BUFFER, size, head, GL_STREAM_DRAW );
}


/*	This function is just call of bind and fill together.
 *  handle - VAO id.
 */
void VBuffer::setup( GLuint handle )
{
	bind( handle );
	fill( );
}

inline void apply_textures( list<GLuint>* textures )
{
	listElement< GLuint >* texture = textures->head;
	for( int index = 0; texture != NULL; texture = texture->next, ++index ){
		glActiveTexture( GL_TEXTURE0 + index );
		glBindTexture( GL_TEXTURE_2D, texture->data );
	}
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

	glEnable(GL_TEXTURE_2D);
	while(handler_element != NULL){
		VBOStructureHandle* vbostructure = handler_element->data;
		apply_textures( &vbostructure->textures );
		apply_material( vbostructure->material, pass );
		glDrawElements( vbostructure->method, vbostructure->count, GL_UNSIGNED_INT, vbostructure->indexes );
		handler_element = handler_element->next;
	}
	glUseProgram( 0 );
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

	if( !v || v->type != prQUADS ||
		v->material != sprite->material ||
		!v->textures.cmp( &sprite->textures ) ){
		v = new VBOStructureHandle( sprite->brush.type, &sprite->textures, sprite->material );
		handler->push_back( v );
	}

	v->set_indexes( sprite->brush.point_index, sprite->brush.points_count );
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
