#include <stdlib.h>
#include <stdio.h>
#include "engine.h"
#include "output.h"
 
class CMyEngine: public CEngine
{
public:
	void AdditionalInit ();
	void Think	    ( const int& iElapsedTime );
	void Render	    ( SDL_Surface* pDestSurface );
 
	void KeyUp  	    (const int& iKeyEnum);
	void KeyDown	    (const int& iKeyEnum);
 
	void MouseMoved     (const int& iButton, 
			     const int& iX, 
			     const int& iY, 
			     const int& iRelX, 
		             const int& iRelY);
 
	void MouseButtonUp  (const int& iButton, 
			     const int& iX, 
			     const int& iY, 
			     const int& iRelX, 
		             const int& iRelY);
 
	void MouseButtonDown(const int& iButton, 
			     const int& iX, 
			     const int& iY, 
			     const int& iRelX, 
		             const int& iRelY);
 
	void WindowInactive();
	void WindowActive();
 
	void End();
};
 
 
// Entry point
int main(int argc, char* argv[])  // <- this must match exactly, since SDL rewrites it
{
    CMyEngine Engine;
    print_d("Strat engine")
    
	Engine.SetTitle( "Loading..." );
	Engine.Init();
 
	Engine.SetTitle( "SDL Testing!" );
	Engine.Start();
 
	Engine.SetTitle( "Quitting..." );
 
	return 0;
}
 
void CMyEngine::AdditionalInit()
{
	// Load up additional data
	
}
 
void CMyEngine::Think( const int& iElapsedTime )
{
	// Do time-based calculations
}
 
void CMyEngine::Render( SDL_Surface* pDestSurface )
{
	// Display slick graphics on screen
}
 
void CMyEngine::KeyDown(const int& iKeyEnum)
{        
    switch (iKeyEnum)
    {
    case SDLK_LEFT:
      // Left arrow pressed
      break;
    case SDLK_RIGHT:
      // Right arrow pressed
      break;
    case SDLK_UP:
      // Up arrow pressed
      break;
    case SDLK_DOWN:
      // Down arrow pressed
      break;
    }
}
 
 
void CMyEngine::KeyUp(const int& iKeyEnum)
{
	switch (iKeyEnum)
	{
	case SDLK_LEFT:
	  // Left arrow released
	  break;
	case SDLK_RIGHT:
	  // Right arrow released
	  break;
	case SDLK_UP:
	  // Up arrow released
	  break;
	case SDLK_DOWN:
	  // Down arrow released
	  break;
	}
}
 
void CMyEngine::MouseMoved(const int& iButton, 
			   const int& iX, 
			   const int& iY, 
			   const int& iRelX, 
			   const int& iRelY)
{
	// Handle mouse movement
 
	// iX and iY are absolute screen positions
	// iRelX and iRelY are screen position relative to last detected mouse movement
}
 
void CMyEngine::MouseButtonUp(const int& iButton, 
			      const int& iX, 
			      const int& iY, 
			      const int& iRelX, 
			      const int& iRelY)
{
	// Handle mouse button released
}
 
void CMyEngine::MouseButtonDown(const int& iButton, 
				const int& iX, 
				const int& iY, 
				const int& iRelX, 
				const int& iRelY)
{
	// Handle mouse button pressed
}
 
void CMyEngine::WindowInactive()
{
	// Pause game
}
 
void CMyEngine::WindowActive()
{
	// Un-pause game
}
 
 
void CMyEngine::End()
{
	// Clean up
}
