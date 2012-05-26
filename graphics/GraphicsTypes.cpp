/*
 * GraphicsTypes.cpp
 *
 *  Created on: 26.05.2012
 *
 * FUUUUUUUUUUUUUUUUUUUUU
 *
 */

#include "graphics/GraphicsTypes.h"
#include "graphics/gl_shader.h"

void Sprite::setFixed()
{
	flags |= 4;
	shader = Shaders::getProgram( "fixed" );
}
