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
	GLuint programs[glpLast];
	ShaderConfigStrings* samplers[glpLast];

	void init_flags( UINT glflags );

	inline void add_flag( UINT glflag ){ init_flags( flags | glflag ); }
	inline void clear_flag( UINT glflag ){ init_flags( flags & ( ~glflag ) ); }
	inline unsigned char is_flag( UINT glflag ){ return flags & glflag; }

};

namespace GLMaterialManager {
	void clean( );
	UINT create( UINT glflags );
	UINT get( UINT glflags );
	const GLMaterial* get_pointer( UINT id );

	inline UINT get_flags( UINT material ){
		const GLMaterial* _mat = GLMaterialManager::get_pointer( material );
		return _mat ? _mat->flags : 0;
	}
	inline UINT add_flag( UINT material, UINT glflags ){
		return get( get_flags(material) | glflags );
	}
	inline UINT clear_flag( UINT material, UINT glflags ){
		return get( get_flags(material) & (~glflags) );
	}
}

#define CHANGE_MATERIAL_FLAG( material, op, flag )		\
	material = GLMaterialManager::op( material, flag );


#endif /* MATERIAL_H_ */
