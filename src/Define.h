#ifndef YDEFINE_H
#define YDEFINE_H

#define JOYSTICKENABLE
#define JSSENS 1000

//Render only visible in window
#define RENDER_VISIBLE
#define TIMEREVENTTICK 10
#define INPUT_INTERVAL 100
#define MILISECONDS 1000.0

#ifndef MAX_PATH
	#define MAX_PATH 4096
#endif

// FIXME: MOVE TO GRAPHICS
#ifdef WIN32
	#undef GL_GLEXT_PROTOTYPES
	#include "GL\glew.h"
#else
	#define GL_GLEXT_PROTOTYPES 1
	#define NO_SDL_GLEXT
	#include "GL/glew.h"
	#include "SDL/SDL_opengl.h"
#endif

#endif //YDEFINE_H
