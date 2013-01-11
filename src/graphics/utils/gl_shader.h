/*
 * gl_shader.h
 *
 *  Created on: 24.05.2012
 *
 * Want a shader? Create and manage it yourself.
 *
 */

#ifndef GL_SHADER_H_
#define GL_SHADER_H_

#include "graphics/GraphicsTypes.h"


enum GLSFlags {
	glsNone = 		0x00,
	glsFirst = 		0x01,
	glsNormals = 	0x02,
	glsLight = 		0x04,
	glsFixed = 		0x08,
	glsGeometry = 	0x10,
	glsAll = 		0x1F,
	glsLast = 		0x20
};


namespace Shaders {
	//GLuint getProgram( std::string filename );
	GLuint getProgram( enum GLSFlags glflags );
	void passUniform3fv( enum GLSFlags, const char* name, int count, float* data);
	void passUniformMatrix4fv( enum GLSFlags, const char* name,	int count, GLboolean transpose, float* data );
}



#endif /* GL_SHADER_H_ */
