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
		GLuint geometry;
		GLuint directional_light;
	} programs;

	void init_flags( UINT glflags );

	inline void add_flag( UINT glflag ){ init_flags( flags | glflag ); }
	inline void clear_flag( UINT glflag ){ init_flags( flags & ( ~glflag ) ); }
	inline unsigned char is_flag( UINT glflag ){ return flags & glflag; }

};

/*
namespace GLMaterialManager {

	void clean( );

	UINT CreateMaterial( UINT glflags );
	GLMaterial* GetMaterial( UINT id );
}
*/


#endif /* MATERIAL_H_ */
