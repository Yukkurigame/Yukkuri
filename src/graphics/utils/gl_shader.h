/*
 * gl_shader.h
 *
 *  Created on: 24.05.2012
 *
 */

#ifndef GL_SHADER_H_
#define GL_SHADER_H_

#include "graphics/GraphicsTypes.h"
#include "graphics/utils/gl_uniforms.h"


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
	glpNone = 0,
	glpDefault,
	glpSimple,
	glpGeometry,
	glpPointLight,
	glpDirLight,
	glpLast,
};


enum GLSLocations {
	gllPosition = 0,
	gllTexCoord = 1,
	gllColor = 2,
	gllNormal = 3
};



struct ShaderConfigAttributes {
	GLint index;
	char* name;

	ShaderConfigAttributes() : index(0), name(NULL) { }
	~ShaderConfigAttributes();
};

/*
struct ShaderConfigStrings {
	unsigned int count;
	char** data;
	ShaderConfigStrings() : count(0), data(NULL) {}
	~ShaderConfigStrings();
};
*/


struct ShaderConfigData {
	char* name;
	char* vertex_name;
	char* fragment_name;
	char** output;
	unsigned int output_count;
	unsigned int attributes_count;
	ShaderConfigAttributes* attributes;
	//ShaderConfigStrings* samplers;

	ShaderConfigData() : name(NULL), vertex_name(NULL), fragment_name(NULL), output(NULL),
						output_count(), attributes_count(), attributes(NULL) {}
						//samplers(NULL)
	~ShaderConfigData();
};


namespace Shaders {

	GLuint getProgram( enum GLSPass, enum GLSFlags glflags );
	//ShaderConfigStrings* getSamplers( enum GLSPass, enum GLSFlags glflags );
	UniformHandlers* getUniforms( enum GLSPass pass, enum GLSFlags glflags );

}



#endif /* GL_SHADER_H_ */
