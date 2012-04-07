
#include "engine.h"
#include "config.h"

#include "Render.h"
#include "sdl_graphics.h"

#include "Bindings.h"

#include "safestring.h"
#include "debug.h"

using namespace Debug;


extern MainConfig conf;

/** Default constructor. **/
CEngine::CEngine( )
{
	LastTick = 0;
	Title = 0;

	FPSTickCounter = 0;
	FPSCounter = 0;
	CurrentFPS = 0;

	Minimized = false;
}

/** Destructor. **/
CEngine::~CEngine()
{
	SDL_Quit();
}

/** Sets the height and width of the window.
 * 	Dummy
**/
void CEngine::SetSize()
{

}

/** Initialize SDL, the window and the additional data. **/
bool CEngine::Init()
{

	debug( MAIN, "Loading defaults.\n" );
	if( !conf.load( ) ){
		debug( MAIN, "Loading default configuration failed. Exiting." );
		return false;
	}


	debug( MAIN, "Initializing SDL...	" );
	// Register SDL_Quit to be called at exit; makes sure things are cleaned up when we quit.
	atexit( SDL_Quit );

	// Initialize SDL's subsystems - in this case, only video.
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 ) {
		debug( NONE, "[FAIL]\n" );
		debug( MAIN, "Couldn't initialize SDL: " + static_cast<std::string>(SDL_GetError( )) + "\n" );
		return false;
	}

	SetTitle( "Loading..." );

	int videoFlags;
	const SDL_VideoInfo *videoInfo;

	videoInfo = SDL_GetVideoInfo( );

	if( !videoInfo ) {
		debug( MAIN, "Video query failed: " + static_cast<std::string>(SDL_GetError( )) + "\n" );
	}

	videoFlags = SDL_OPENGL; // Enable OpenGL in SDL
	videoFlags |= SDL_HWPALETTE; // Store the palette in hardware

	RenderManager::Instance()->openglInit( );

	// Attempt to create a window with the specified height and width.
	if( !SDLGraphics::SetScreen( SDL_SetVideoMode( conf.windowWidth, conf.windowHeight, 0, videoFlags ) ) ) {
		debug( NONE, "[FAIL]\n" );
		debug( MAIN, "Unable to set up video: " + static_cast<std::string>(SDL_GetError( )) + "\n" );
		return false;
	}

	RenderManager::Instance()->openglSetup( conf.windowWidth, conf.windowHeight );

	debug( NONE, "Done\n" );

	debug( MAIN, "Load sprites.\n" );

	if( !RenderManager::Instance()->LoadTextures( ) ){
		debug( MAIN, "Sprites loading failed." );
		return false;
	}


#ifdef JOYSTICKENABLE
	if( SDL_NumJoysticks() > 0 ){
		int jnum = SDL_NumJoysticks();
		char d[2]; //100 joystics, lol
		snprintf( d, 2, "%d", jnum );
		debug( MAIN, (std::string)(d) + " joysticks were found:\n" );
		for( int i=0; i < jnum; i++ )
			debug( MAIN, static_cast<std::string>(SDL_JoystickName(i)) + "\n" );
		SDL_JoystickEventState(SDL_ENABLE);
		joystick = SDL_JoystickOpen(0);
	}
#else
	debug( MAIN, "Joystick not enabled.\n" );
#endif

	if(	!AdditionalInit() ){
		return false;
	}

	return true;
}

/** The main loop. **/
void CEngine::Start()
{
	float UpdIterations = 0.0f;
	float CyclesLeft = 0.0f;
	LastTick = 0;
	EndLoop = false;
	long ElapsedTicks;
	float UpdateInterval;
	float MaxCycles;

	UpdateInterval = 1.0f / conf.maxFrameRate;
	MaxCycles = conf.maxFrameRate / conf.minFrameRate;

	// Main loop.
	while( !EndLoop ){

		Tick = SDL_GetTicks();
		if(Tick - FPSTickCounter >= 250){
			float seconds = (Tick - FPSTickCounter) / 1000.0f;
			CurrentFPS = FPSCounter / seconds;
			FPSTickCounter = Tick;
			FPSCounter = 0;
		}

		ElapsedTicks = Tick - LastTick;

		// Handle mouse and keyboard input
		HandleInput();

		if( Minimized ){
			// Release some system resources if the app. is minimized.
			//WaitMessage(); // pause the application until focus in regained
		}else{
			UpdIterations = ElapsedTicks + CyclesLeft;

			if( UpdIterations > ( MaxCycles * UpdateInterval ) )
				UpdIterations = MaxCycles * UpdateInterval;

			// Do some thinking
			while (UpdIterations > UpdateInterval) { // Update game state a variable number of times
				UpdIterations -= UpdateInterval;
				DoThink( ElapsedTicks );
			}

			CyclesLeft = UpdIterations;
			LastTick = Tick;

			// Render stuff
			DoRender();
		}
	}

	End();
}

void CEngine::Quit( )
{
	EndLoop = true;
}

/** Handles all controller inputs.
	@remark This function is called once per frame.
**/
void CEngine::HandleInput()
{
	// Poll for events, and handle the ones we care about.
	SDL_Event event;

	while( SDL_PollEvent( &event ) )
	{
		int evnt = 0;
		short down = 1;
		switch( event.type ){

			case SDL_QUIT:
				Quit( );
				break;

			case SDL_ACTIVEEVENT:
				if( event.active.state & SDL_APPACTIVE ){
					if( event.active.gain ){
						Minimized = false;
						WindowActive( );
					}else{
						Minimized = true;
						WindowInactive( );
					}
				}
				break;

			case SDL_KEYUP:
				down = 0;	//Yes, no break.
			case SDL_KEYDOWN:
				evnt = event.key.keysym.sym;
				break;			//It's break for both SDL_KEY events.

#ifdef JOYSTICKENABLE
			case SDL_JOYAXISMOTION:
				evnt = SDLK_LAST;
				if( event.jaxis.axis == 0 ){ // Left-right movement
					//left, right, release
					if( event.jaxis.value > JSSENS )
						++evnt;
					else if( event.jaxis.value < -JSSENS )
						evnt +=2;
					else if( event.jaxis.value == 0){
						++evnt;
						down = 0;
					}
				}
				if( event.jaxis.axis == 1 ){ //Up-Down movement
					//up, down, release
					evnt += 2;
					if( event.jaxis.value > JSSENS )
						++evnt;
					else if( event.jaxis.value < -JSSENS )
						evnt +=2;
					else if( event.jaxis.value == 0){
						++evnt;
						down = 0;
					}
				}
				break;

			case SDL_JOYBUTTONUP:
				down = 0;
			case SDL_JOYBUTTONDOWN:
				evnt = SDLK_LAST + 7 + event.jbutton.button;
				break;
#endif

				/*
				case SDL_MOUSEMOTION:
					MouseMoved(
						event.button.button,
						event.motion.x,
						event.motion.y,
						event.motion.xrel,
						event.motion.yrel);
					break;

				case SDL_MOUSEBUTTONUP:
					MouseButtonUp(
						event.button.button,
						event.motion.x,
						event.motion.y,
						event.motion.xrel,
						event.motion.yrel);
					break;

				case SDL_MOUSEBUTTONDOWN:
					MouseButtonDown(
						event.button.button,
						event.motion.x,
						event.motion.y,
						event.motion.xrel,
						event.motion.yrel);
					break;
				*/

		} // switch

		if( evnt > 0 )
			Bindings::bnd.process( evnt, down );

	} // while (handling input)

}

/** Handles the updating routine. **/
void CEngine::DoThink( const int& ElapsedTicks )
{
	Think( ElapsedTicks );
}

/** Handles the rendering and FPS calculations. **/
void CEngine::DoRender()
{
	++FPSCounter;

	Render(  );

	SDL_Delay(1);
}

/** Sets the title of the window
	@param title A character array that contains the text that the window title should be set to.
**/
void CEngine::SetTitle(const char* title)
{
	Title = title;
	SDL_WM_SetCaption( Title, 0 );
}

/** Retrieve the title of the application window.
	@return The last set windows title as a character array.
	@remark Only the last set title is returned. If another application has changed the window title, then that title won't be returned.
**/
const char* CEngine::GetTitle()
{
	return Title;
}

/** Get the current FPS.
	@return The number of drawn frames in the last second.
	@remark The FPS is only updated once each second.
**/
float CEngine::GetFPS()
{
	return CurrentFPS;
}

char* CEngine::GetFPSText()
{
	snprintf( FPStext, 10, "FPS: %0.1f", GetFPS() );
	return FPStext;
}

int CEngine::getScreenW()
{
	return conf.windowWidth;
}

int CEngine::getScreenH()
{
	return conf.windowHeight;
}
