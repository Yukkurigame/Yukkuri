/*
 * Material.cpp
 *
 *  Created on: 06.01.2013
 */


#include "graphics/sprite/Material.h"
#include "graphics/utils/gl_shader.h"


void GLMaterial::init_flags( UINT glflags )
{
	flags = glflags;
	enum GLSFlags new_flags = static_cast<enum GLSFlags>(glflags);
	for( unsigned int i = glpNone; i < glpLast; ++i ){
		enum GLSPass pass = static_cast<enum GLSPass>(i);
		programs[i] = Shaders::getProgram( pass, new_flags );
		//samplers[i] = Shaders::getSamplers( pass, new_flags );
		uniforms[i] = Shaders::getUniforms( pass, new_flags );
	}
}

#undef GET_PROGRAM



/////////////////////////////////////////////////////
//			Material manager
namespace GLMaterialManager
{
	GLMaterial* materials = NULL;
	unsigned int materials_count = 0;

}

void GLMaterialManager::clean( )
{
	if( materials )
		free( materials );
}


UINT GLMaterialManager::create( UINT params )
{
	materials_count++;
	materials = (GLMaterial*)realloc(materials, (unsigned)sizeof(GLMaterial) * materials_count );
	materials[materials_count - 1].flags = 0;
	materials[materials_count - 1].init_flags( params );
	return materials_count - 1;
}


UINT GLMaterialManager::get( UINT glflags )
{
	for( unsigned int i = 0; i < materials_count; ++i ){
		if( materials[i].flags == glflags )
			return i;
	}
	return create( glflags );
}

const GLMaterial* GLMaterialManager::get_pointer( UINT id )
{
	if( id > materials_count )
		return NULL;
	return &materials[id];
}
