#include "engine.h"
 
/** Default constructor. **/
CEngine::CEngine()
{
	m_lLastTick = 0;
	m_czTitle = 0;
 
	m_iFPSTickCounter = 0;
	m_iFPSCounter = 0;
	m_iCurrentFPS = 0;
	m_sFPStext = new char[16];
 
	m_bMinimized = false;
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

	Graphics::graph.openglInit( );


	//SetSize( );


	// Attempt to create a window with the specified height and width.
	// If we fail, return error.
	if ( !SDL_SetVideoMode( WWIDTH, WHEIGHT, 0, SDL_OPENGL ) ) {
		cout << "[FAIL]" << endl;
		cout << "Unable to set up video: " << SDL_GetError() << endl;
		exit( 1 );
	}

	Graphics::graph.openglSetup( WWIDTH, WHEIGHT );

	cout << "Done" << endl;
   
	if( SDL_NumJoysticks() > 0 ){
		cout << SDL_NumJoysticks() << " joysticks were found:" << endl;
		for( int i=0; i < SDL_NumJoysticks(); i++ )
			cout << SDL_JoystickName(i) << endl;
		SDL_JoystickEventState(SDL_ENABLE);
		joystick = SDL_JoystickOpen(0);
	}

	AdditionalInit();

}
 
/** The main loop. **/
void CEngine::Start()
{
	float m_dUpdIterations = 0.0f;
	float m_sdCyclesLeft = 0.0f;
	m_lLastTick = 0;
	m_bQuit = false;
	player_movex = 0;
	player_movey = 0;
	long iElapsedTicks;

	// Main loop: loop forever.
	while ( !m_bQuit )
	{
		m_lTick = SDL_GetTicks();
		iElapsedTicks = m_lTick - m_lLastTick;

		// Handle mouse and keyboard input
		HandleInput();
 
		if ( m_bMinimized ) {
			// Release some system resources if the app. is minimized.
			//WaitMessage(); // pause the application until focus in regained
		} else {
			m_dUpdIterations = (( iElapsedTicks ) + m_sdCyclesLeft);

			if (m_dUpdIterations > (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL))
				m_dUpdIterations = (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL);

			// Do some thinking
			while (m_dUpdIterations > UPDATE_INTERVAL) { // Update game state a variable number of times
				m_dUpdIterations -= UPDATE_INTERVAL;
				DoThink( iElapsedTicks );
			}

			m_sdCyclesLeft = m_dUpdIterations;
			m_lLastTick =  m_lTick;

			// Render stuff
			DoRender();
		}
	}
 
	End();
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
		switch ( event.type )
		{
		case SDL_KEYDOWN:
			// If escape is pressed set the Quit-flag
			if( event.key.keysym.sym == SDLK_ESCAPE )
			{
				m_bQuit = true;
				break;
			}

			if( event.key.keysym.sym == SDLK_s )
			{
				//string name = "screenshot.bmp";
				Graphics::graph.SaveScreenshot("screenshot.bmp");
			}
 
			KeyDown( event.key.keysym.sym );
			break;
 
		case SDL_KEYUP:
			KeyUp( event.key.keysym.sym );
			break;
 
 
		case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
				if( event.jaxis.axis == 0) /* Left-right movement */
				{
					//TODO: it's cruve
					player_movex = ( event.jaxis.value > 0 ? 1 : (event.jaxis.value < 0 ? -1 : 0));
				}
				if( event.jaxis.axis == 1)  /* Up-Down movement */
				{
					player_movey = ( event.jaxis.value > 0 ? 1 : (event.jaxis.value < 0 ? -1 : 0));
				}
			break;

		case SDL_QUIT:
			m_bQuit = true;
			break;
 
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
 
		case SDL_ACTIVEEVENT:
			if ( event.active.state & SDL_APPACTIVE ) {
				if ( event.active.gain ) {
					m_bMinimized = false;
					WindowActive();
				} else {
					m_bMinimized = true;
					WindowInactive();
				}
			}
			break;
		} // switch
	} // while (handling input)

}

/** Handles the updating routine. **/
void CEngine::DoThink( const int& iElapsedTicks ) 
{
	Think( iElapsedTicks );
 
	m_iFPSTickCounter += iElapsedTicks;
}
 
/** Handles the rendering and FPS calculations. **/
void CEngine::DoRender()
{
	++m_iFPSCounter;
	if ( m_iFPSTickCounter >= 1000 )
	{
		m_iCurrentFPS = m_iFPSCounter;
		m_iFPSCounter = 0;
		m_iFPSTickCounter = 0;
	}
 
	Render(  );

	SDL_Delay(1);
}
 
/** Sets the title of the window 
	@param czTitle A character array that contains the text that the window title should be set to.
**/
void CEngine::SetTitle(const char* czTitle)
{
	m_czTitle = czTitle;
	SDL_WM_SetCaption( czTitle, 0 );
}
 
/** Retrieve the title of the application window.
	@return The last set windows title as a character array.
	@remark Only the last set title is returned. If another application has changed the window title, then that title won't be returned.
**/
const char* CEngine::GetTitle()
{
	return m_czTitle;
}
 
/** Get the current FPS.
	@return The number of drawn frames in the last second.
	@remark The FPS is only updated once each second.
**/
int CEngine::GetFPS()
{
	return m_iCurrentFPS;
}

char* CEngine::GetFPSText()
{
	sprintf( m_sFPStext, "FPS: %d", GetFPS() );
	return m_sFPStext;
}


