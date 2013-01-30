/*
 * gl_uniforms.cpp
 *
 *  Created on: 25.01.2013
 *
 */

#include "Define.h"
#include "graphics/utils/gl_uniforms.h"
#include <cstdlib>
#include "safestring.h"


namespace UniformsManager {

	UniformData* uniforms;
	short* uniforms_updated;
	UINT uniforms_count;
}


/* Register new uniform and get its id.
 * This function searches for uniform in already registred items nad returns it if found.
 * name - name of uniform
 * type - type of uniform
 * returns uniform id
 */
UINT UniformsManager::register_uniform( const char* name, UINT type )
{
	// Search for already registred uniforms
	for( UINT i = 0; i < uniforms_count; ++i ){
		if( !strcmp( uniforms[i].name, name ) )
			return i;
	}
	// Create new uniform entry
	int index = uniforms_count++;
	uniforms = (UniformData*)realloc( uniforms, (UINT)sizeof(UniformData) * uniforms_count );
	uniforms[index].name = strdup(name);
	uniforms[index].type = type;
	uniforms[index].data = NULL;
	// Add entry of uniform status
	uniforms_updated = (short*)realloc( uniforms_updated, (UINT)sizeof(short) * uniforms_count );
	uniforms_updated[index] = false;
	return index;
}


/* Get uinform description
 * index - index of uniform
 */
const UniformData* UniformsManager::get_uniform( UINT index )
{
	if( index > uniforms_count )
		return NULL;
	return &uniforms[index];
}


/* Pass data location to uniform
 * This function passes data location to uniform
 * id - uniform id
 * data - data
 */
void UniformsManager::pass_data( UINT id, void* data )
{
	if( id > uniforms_count )
		return;

	uniforms[id].data = data;
}


/* This function sends data to gl, using perviously passed data pointer
 * location - location of uniform
 * id - uniform id
 * count - count of uniforms in data
 */
void UniformsManager::send_data( int location, UINT id, int count )
{
	if( location < 0 || id > uniforms_count )
		return;

	UniformData* uniform = &uniforms[id];
	if( uniform->data == NULL )
		return;

	switch( uniform->type ){
		case GL_FLOAT:
			glUniform1f( location, *(float*)uniform->data );
			break;

		case GL_INT:
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
			glUniform1i( location, *(int*)uniform->data );
			break;

		case GL_FLOAT_VEC2:
			glUniform2fv( location, count, (float*)uniform->data );
			break;

		case GL_FLOAT_VEC3:
			glUniform3fv( location, count, (float*)uniform->data );
			break;

		case GL_INT_VEC4:
			glUniform4iv( location, count, (int*)uniform->data );
			break;

		case GL_FLOAT_MAT4:
			glUniformMatrix4fv( location, count, GL_FALSE, (float*)uniform->data );
			break;
	}
}
