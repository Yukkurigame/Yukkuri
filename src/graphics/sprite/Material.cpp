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
	programs.geometry = Shaders::getProgram( glpGeometry, static_cast<enum GLSFlags>(glflags) );
	programs.directional_light = Shaders::getProgram( glpDirLight, static_cast<enum GLSFlags>(glflags) );
}



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
