#include "engine.h"

/** Default constructor. **/
CEngine::CEngine()
{
	LastTick = 0;
	Title = 0;

	FPSTickCounter = 0;
	FPSCounter = 0;
	CurrentFPS = 0;
	FPStext = new char[16];

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
void CEngine::Init()
{
	cout << "Initializing SDL...	";
	// Register SDL_Quit to be called at exit; makes sure things are cleaned up when we quit.
	atexit( SDL_Quit );

	// Initialize SDL's subsystems - in this case, only video.
	if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 ) {
		cout << "[FAIL]" << endl;
		cout << "Couldn't initialize SDL: " << SDL_GetError() << endl;
		exit( 1 );
	}

	SetTitle( "Loading..." );

	Graphics::Instance()->openglInit( );

	// Attempt to create a window with the specified height and width.
	// If we fail, return error.
	if ( !Graphics::Instance()->SetScreen( SDL_SetVideoMode( WWIDTH, WHEIGHT, 0, SDL_OPENGL ) ) ) {
		cout << "[FAIL]" << endl;
		cout << "Unable to set up video: " << SDL_GetError() << endl;
		exit( 1 );
	}

	Graphics::Instance()->openglSetup( WWIDTH, WHEIGHT );

	cout << "Done" << endl;

#ifdef JOYSTICKENABLE
	if( SDL_NumJoysticks() > 0 ){
		cout << SDL_NumJoysticks() << " joysticks were found:" << endl;
		for( int i=0; i < SDL_NumJoysticks(); i++ )
			cout << SDL_JoystickName(i) << endl;
		SDL_JoystickEventState(SDL_ENABLE);
		joystick = SDL_JoystickOpen(0);
	}
#else
	cout << "Joystick not enabled." << endl;
#endif


	AdditionalInit();

}

/** The main loop. **/
void CEngine::Start()
{
	float UpdIterations = 0.0f;
	float CyclesLeft = 0.0f;
	LastTick = 0;
	EndLoop = false;
	long ElapsedTicks;

	// Main loop.
	while( !EndLoop ){

		Tick = SDL_GetTicks();
		if(Tick - FPSTickCounter >= 250){
		    float seconds = (Tick - FPSTickCounter) / 1000.0;
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

			if (UpdIterations > (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL))
				UpdIterations = (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL);

			// Do some thinking
			while (UpdIterations > UPDATE_INTERVAL) { // Update game state a variable number of times
				UpdIterations -= UPDATE_INTERVAL;
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

	while ( SDL_PollEvent( &event ) )
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
				if( event.jaxis.axis == 0){ // Left-right movement
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
				if( event.jaxis.axis == 1){ //Up-Down movement
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
	sprintf( FPStext, "FPS: %0.1f", GetFPS() );
	return FPStext;
}
