/*
 * Material.h
 *
 *  Created on: 06.01.2013
 */

#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "Define.h"
#include "graphics/utils/gl_shader.h"	// For flags
#include "types.h"


struct GLMaterial
{
	UINT flags;
	struct  {
		GLuint base;
		GLuint simple;
		GLuint geometry;
		GLuint directional_light;
	} programs;

	struct {
		ShaderConfigStrings* base;
		ShaderConfigStrings* simple;
		ShaderConfigStrings* geometry;
		ShaderConfigStrings* directional_light;
	} samplers;

	void init_flags( UINT glflags );

	inline void add_flag( UINT glflag ){ init_flags( flags | glflag ); }
	inline void clear_flag( UINT glflag ){ init_flags( flags & ( ~glflag ) ); }
	inline unsigned char is_flag( UINT glflag ){ return flags & glflag; }

};

namespace GLMaterialManager {
	void clean( );
	UINT create( UINT glflags );
	UINT get( UINT glflags );
	GLMaterial* get_pointer( UINT id );
}

// You want beautiful-look functions instead? Write it yourself.
#define CHANGE_MATERIAL_FLAG( material, op, flag )					\
	GLMaterial* _mat = GLMaterialManager::get_pointer( material );	\
	if( _mat )														\
		_mat->op( flag );


#endif /* MATERIAL_H_ */
