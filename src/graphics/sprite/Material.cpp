/*
 * Material.cpp
 *
 *  Created on: 06.01.2013
 */


#include "graphics/sprite/Material.h"
#include "graphics/utils/gl_shader.h"

#define GET_PROGRAM( target, val )	\
	programs.target = Shaders::getProgram( val, static_cast<enum GLSFlags>(glflags) );
#define GET_SAMPLERS( target, val )	\
	samplers.target = Shaders::getSamplers( val, static_cast<enum GLSFlags>(glflags) );

void GLMaterial::init_flags( UINT glflags )
{
	flags = glflags;
	GET_PROGRAM( base, glpDefault )
	GET_PROGRAM( simple, glpSimple )
	GET_PROGRAM( geometry, glpGeometry )
	GET_PROGRAM( directional_light, glpDirLight )
	GET_SAMPLERS( base, glpDefault )
	GET_SAMPLERS( simple, glpSimple )
	GET_SAMPLERS( geometry, glpGeometry )
	GET_SAMPLERS( directional_light, glpDirLight )
}

#undef GET_PROGRAM



/////////////////////////////////////////////////////
//			Material manager
/*
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


UINT GLMaterialManager::CreateMaterial( UINT params )
{
	materials_count++;
	materials = (GLMaterial*)realloc(materials, sizeof(GLMaterial*) * materials_count );
	materials[materials_count - 1].init_flags( params );
	return materials_count;
}


GLMaterial* GLMaterialManager::GetMaterial( UINT id )
{
	if( id > materials_count )
		return NULL;
	return &materials[id];
}
*/
