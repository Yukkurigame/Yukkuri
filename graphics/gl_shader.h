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

#include <string>
#include "graphics/GraphicsTypes.h"

namespace Shaders {
	GLuint getProgram( std::string filename );
}



#endif /* GL_SHADER_H_ */
