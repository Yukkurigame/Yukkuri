#include "engine.h"
 
/** Default constructor. **/
CEngine::CEngine()
{
    m_lLastTick        = 0;
    //m_iWidth        = 800;
    //m_iHeight        = 600;
    m_czTitle        = 0;
 
    m_pScreen        = 0;
    m_pFPSDisp        = 0;
 
    m_iFPSTickCounter    = 0;
    m_iFPSCounter        = 0;
    m_iCurrentFPS        = 0;
    m_bFPStoggle        = true;
    m_sFPStext            = new char[16];
 
    m_bMinimized        = false;
}
 
/** Destructor. **/
CEngine::~CEngine()
{
        TTF_CloseFont( m_pFPSFont );
        TTF_Quit();
    SDL_Quit();
}
 
/** Sets the height and width of the window.
    @param iWidth The width of the window
    @param iHeight The height of the window
**/
void CEngine::SetSize()
{
    //putenv("SDL_VIDEODRIVER=fbcon") ;
    m_pScreen = SDL_SetVideoMode( WWIDTH, WHEIGHT, 0, SDL_SWSURFACE);
}
 
/** Initialize SDL, the window and the additional data. **/
void CEngine::Init()
{
    cout << "Initializing SDL...    ";
    // Register SDL_Quit to be called at exit; makes sure things are cleaned up when we quit.
    atexit( SDL_Quit );

    // Initialize SDL's subsystems - in this case, only video.
    if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 ) {
        cout << "[FAIL]" << endl;
        cout << "Couldn't initialize SDL: " << SDL_GetError() << endl;
        exit( 1 );
    }
    
    // Attempt to create a window with the specified height and width.
    SetSize( );
 
    // If we fail, return error.
    if ( m_pScreen == NULL ) {
        cout << "[FAIL]" << endl;
        cout << "Unable to set up video: " << SDL_GetError() << endl;
        exit( 1 );
    }
    
    if( TTF_Init() < 0 ) {
        cout << "[FAIL]" << endl;
        cout << "Unable to initialize SDL_ttf: " << SDL_GetError() << endl;
        exit( 1 );   
    }
    
    m_pFPSFont = TTF_OpenFont( "data/shared/Monospace.ttf", 15 );
    
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
    
    // Main loop: loop forever.
    while ( !m_bQuit )
    {
        m_lTick = SDL_GetTicks();
        long iElapsedTicks = m_lTick - m_lLastTick;
        
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
            	string name = "screenshot.bmp";            	
            	SDL_SaveBMP( m_pScreen, name.c_str());
            }
 
            KeyDown( event.key.keysym.sym );
            break;
 
        case SDL_KEYUP:
            KeyUp( event.key.keysym.sym );
            break; 
 
 
        case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
                if( event.jaxis.axis == 0) /* Left-right movement */
                {
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

/**Display framerate**/
void CEngine::DisplayFPS()
{
    SDL_Rect offset;
    SDL_Color tColor = { 255, 0, 0 };

    sprintf( m_sFPStext, "FPS: %d", GetFPS() );
    m_pFPSDisp = TTF_RenderText_Solid( m_pFPSFont, m_sFPStext, tColor );
    offset.x = 0;
    offset.y = 10;

    SDL_BlitSurface( m_pFPSDisp, NULL, m_pScreen, &offset );
} 

 
/** Handles the updating routine. **/
void CEngine::DoThink( const int& iElapsedTicks ) 
{
    //long iElapsedTicks = SDL_GetTicks() - m_lLastTick;
    //m_lLastTick = SDL_GetTicks();
 
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
 
    SDL_FillRect( m_pScreen, 0, SDL_MapRGB( m_pScreen->format, 0, 150, 0 ) );
 
    // Lock surface if needed
    if ( SDL_MUSTLOCK( m_pScreen ) )
        if ( SDL_LockSurface( m_pScreen ) < 0 )
            return;
 
    Render( GetSurface() );
    
    if( m_bFPStoggle )
        DisplayFPS();
    
    // Unlock if needed
    if ( SDL_MUSTLOCK( m_pScreen ) ) 
        SDL_UnlockSurface( m_pScreen );
 
    // Tell SDL to update the whole gScreen
    SDL_Flip( m_pScreen );
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
 
/** Retrieve the main screen surface.
    @return A pointer to the SDL_Surface surface
    @remark The surface is not validated internally.
**/
SDL_Surface* CEngine::GetSurface()
{ 
    return m_pScreen;
}
 
/** Get the current FPS.
    @return The number of drawn frames in the last second.
    @remark The FPS is only updated once each second.
**/
int CEngine::GetFPS()
{
    return m_iCurrentFPS;
}