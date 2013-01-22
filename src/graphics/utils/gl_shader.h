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
	glsAll = 		0x0F,
	glsLast = 		0x10
};


enum GLSPass {
	glpNone = 0x00,
	glpDefault = 0x01,
	glpSimple = 0x02,
	glpGeometry = 0x04,
	glpPointLight = 0x8,
	glpDirLight = 0x10,
	glpLast = 0x20,
};

enum GLSLocations {
	gllPosition = 0,
	gllTexCoord = 1,
	gllColor = 2,
	gllNormal = 3
};


struct ShaderConfigAttributes {
	GLuint index;
	char* name;

	ShaderConfigAttributes() : index(0), name(NULL) { }
	~ShaderConfigAttributes();
};


struct ShaderConfigData {
	char* name;
	char* vertex_name;
	char* fragment_name;
	char** output;
	unsigned int output_count;
	unsigned int attributes_count;
	ShaderConfigAttributes* attributes;

	ShaderConfigData() : name(NULL), vertex_name(NULL), fragment_name(NULL), output(NULL),
						output_count(), attributes_count(), attributes(NULL) {}
	~ShaderConfigData();
};


namespace Shaders {
	//GLuint getProgram( std::string filename );
	GLuint getProgram( enum GLSPass, enum GLSFlags glflags );
	void passUniform2fv( enum GLSFlags, const char* name, int count, float* data);
	void passUniform3fv( enum GLSFlags, const char* name, int count, float* data);
	void passUniformMatrix4fv( enum GLSFlags, const char* name,	int count, GLboolean transpose, float* data );
}



#endif /* GL_SHADER_H_ */
