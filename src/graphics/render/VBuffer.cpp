/*
 * VBuffer.cpp
 *
 *  Created on: 21.01.2013
 *
 */

#include "graphics/render/VBuffer.h"
#include "graphics/utils/VBOArray.h"


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


/*	This function draws vertex array object
 * 	vbostructure - linked list of vao description
 */
void VBuffer::draw( VBOStructureHandle* vbostructure )
{
	GLuint aprog = 0;
	GLuint texture = 0;
	GLuint normals = 0;

	glEnable(GL_TEXTURE_2D);
	while(vbostructure != NULL){
		if( aprog != vbostructure->shader ){
			aprog = vbostructure->shader;
			glUseProgram( aprog );
		}
		if( texture != vbostructure->atlas ){
			texture = vbostructure->atlas;
			glActiveTexture( GL_TEXTURE_FROM_INDEX(gltColor) );
			glBindTexture( GL_TEXTURE_2D, texture );
		}
		if( normals != vbostructure->normals ){
			normals = vbostructure->normals;
			glActiveTexture( GL_TEXTURE_FROM_INDEX(gltNormal) );
			glBindTexture( GL_TEXTURE_2D, normals );
		}
		glDrawElements( vbostructure->method, vbostructure->count, GL_UNSIGNED_INT, vbostructure->indexes );
		//glDrawArrays(GL_QUADS, vbostructure->indexes, vbostructure->count);
		//Clean vbos
		vbostructure = vbostructure->next;
	}
	glUseProgram( 0 );
	glActiveTexture( GL_TEXTURE_FROM_INDEX(gltColor) );
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture( GL_TEXTURE_FROM_INDEX(gltNormal) );
	glBindTexture(GL_TEXTURE_2D, 0);

#ifdef DEBUG_DRAW_RECTANGLES
	for( int i = 0; i < count; i = i + 4 )
		glDrawArrays(GL_LINE_LOOP, i, 4);
#endif

	glDisable(GL_TEXTURE_2D);
}




inline void vbo_handler( Sprite* s, GLuint shader, VBOStructureHandle*& v, VBOStructureHandle*& first )
{
	if( s == NULL || !s->isVisible() )
		return;

	if( !v ){
		first = v = new VBOStructureHandle( s->brush.type, s->atlas, s->normals, shader );
	}else if( v->type != prQUADS || s->atlas != v->atlas ||
			s->normals != v->normals || shader != v->shader ){
		v->next = new VBOStructureHandle( s->brush.type, s->atlas, s->normals, shader );
		v = v->next;
	}

	v->set_indexes( s->brush.point_index, s->brush.points_count );
}


/*	This function make vao array from sprite array
 *	pass - id of render pass.
 *	sprites - array of sprites
 *	returns pointer to the first vbo info handler
 */
VBOStructureHandle* VBuffer::prepare_handler( int pass, list< Sprite* >* sprites )
{
	VBOStructureHandle* v = NULL;
	VBOStructureHandle* first = NULL;
	listElement< Sprite* >* sprites_element = sprites->head;

#define CASED_PROGAM( d, location )				\
	case d:									\
		shader = s->material.programs.location;	\
		break;

	while( sprites_element != NULL ){
		Sprite* s = sprites_element->data;
		int shader = glpNone;
		switch(pass){
			CASED_PROGAM( glpDefault, base )
			CASED_PROGAM( glpSimple, simple )
			CASED_PROGAM( glpGeometry, geometry )
			CASED_PROGAM( glpDirLight, directional_light )
		}
		vbo_handler( s, shader, v, first );

		sprites_element = sprites_element->next;
	}

#undef CASED_PROGAM

	return first;
}


/*	This function make vao array from sprite array
 *	sprites - Pointer to array of sprites
 *	scount - count of sprites in array
 *	returns pointer to the first vbo info handler
 */
VBOStructureHandle* VBuffer::prepare_handler( Sprite* sprites, unsigned int scount )
{
	//int count = 0;
	VBOStructureHandle* v = NULL;
	VBOStructureHandle* first = NULL;
	for( unsigned int i = 0; i < scount; ++i ){
		Sprite* s = &sprites[i];
		vbo_handler( s, glpSimple, v, first );
	}
	return first;
}


/*	This function removes all elements from vao handler
 *	handler - Pointer to first handler element
 */
void VBuffer::free_handler( VBOStructureHandle** handler )
{
	VBOStructureHandle* first = *handler;
	VBOStructureHandle* temp;
	while( first != NULL ){
		temp = first;
		first = first->next;
		delete temp;
	}
	handler = NULL;
}
