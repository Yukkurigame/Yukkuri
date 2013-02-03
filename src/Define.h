#ifndef YDEFINE_H
#define YDEFINE_H

#define JOYSTICKENABLE
#define JSSENS 1000

//Render only visible in window
#define RENDER_VISIBLE
#define TIMEREVENTTICK 10
#define INPUT_INTERVAL 100
#define MILISECONDS 1000.0


#if defined(WIN32)
	#define SEP '\\'
	#define ALTSEP '/'
	#define MAXPATHLEN 256
#endif

/* Filename separator */
#ifndef SEP
	#define SEP '/'
#endif

#ifndef MAXPATHLEN
	#if defined(PATH_MAX) && PATH_MAX > 1024
		#define MAXPATHLEN PATH_MAX
	#else
		#define MAXPATHLEN 1024
	#endif
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
