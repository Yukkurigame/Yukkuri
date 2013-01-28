/*
 * graphics.cpp
 *
 *  Created on: 27.11.2012
 */




#include "core/yukkuri.h"
#include "graphics/Render.h"
#include "graphics/render/Textures.h"
//#include "graphics/utils/sdl_graphics.h"
#include "graphics/daytime.h"
#include "config.h"
#include "debug.h"

#include "SDL/SDL.h"


extern MainConfig conf;


bool Yukkuri::InitGraphics( )
{
	Debug::debug( Debug::MAIN, "Initializing SDL...	" );
	// Register SDL_Quit to be called at exit; makes sure things are cleaned up when we quit.
	atexit( SDL_Quit );

	// Initialize SDL's subsystems - in this case, only video.
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 ) {
		Debug::debug( Debug::NONE, "[FAIL]\n" );
		Debug::debug( Debug::MAIN, "Couldn't initialize SDL: " + static_cast<std::string>(SDL_GetError( )) + "\n" );
		return false;
	}

	SetTitle( "Loading..." );

	int videoFlags;
	const SDL_VideoInfo *videoInfo;

	videoInfo = SDL_GetVideoInfo( );

	if( !videoInfo ) {
		Debug::debug( Debug::MAIN, "Video query failed: " + static_cast<std::string>(SDL_GetError( )) + "\n" );
	}

	videoFlags = SDL_OPENGL; // Enable OpenGL in SDL
	videoFlags |= SDL_HWPALETTE; // Store the palette in hardware

	RenderManager::init();
	RenderManager::openglInit( );

	// Attempt to create a window with the specified height and width.
	if( !SDL_SetVideoMode(
			conf.video.windowWidth, conf.video.windowHeight, 0, videoFlags ) ) {
		Debug::debug( Debug::NONE, "[FAIL]\n" );
		Debug::debug( Debug::MAIN, "Unable to set up video: " + static_cast<std::string>(SDL_GetError( )) + "\n" );
		return false;
	}
	GLenum err = glewInit();
	if( err != GLEW_OK ){
		Debug::debug( Debug::GRAPHICS, "Unable to init glew. No YOBA available.\n" );
		return false;
	}

	Window.width = conf.video.windowWidth;
	Window.height = conf.video.windowHeight;

	if( !RenderManager::openglSetup( Window.width, Window.height ) )
		return false;

	Debug::debug( Debug::NONE, "Done\n" );

	Debug::debug( Debug::MAIN, "Load sprites.\n" );

	if( !Textures::load() ){
		Debug::debug( Debug::MAIN, "Sprites loading failed.\n" );
		return false;
	}

	DayTime::init();

#ifdef DEBUG
	// Make explicit swap call to split initialization and rendering
	SDL_GL_SwapBuffers();
#endif

	return true;
}

void Yukkuri::CleanGraphics()
{
	RenderManager::clean();
	DayTime::clean();
	SDL_Quit();
}


void Yukkuri::Render( )
{
	RenderManager::CleanGLScene( );
	RenderManager::DrawGLScene( );
}

void Yukkuri::SetTitle( const char* title )
{
	Window.title = title;
	SDL_WM_SetCaption( Window.title, 0 );
}


const char* Yukkuri::GetTitle()
{
	return Window.title;
}
