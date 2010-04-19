#include <stdlib.h>
#include <stdio.h>
#include "yukkuri.h"
//#include "output.h"
 

// Entry point
int main(int argc, char* argv[])  // <- this must match exactly, since SDL rewrites it
{
    Yukkuri Engine;
    //print_d( "Strat engine" );
    
    Engine.Init();
 
    Engine.SetTitle( "Yukkuri game" );
    Engine.Start();
 
    Engine.SetTitle( "Quitting..." );
 
    return 0;
}
 

void Yukkuri::AdditionalInit()
{
    // Load up additional data    
    img = LoadImage( "data/images/reimu_face_0.png" );
    if( !img ) {
        cout << endl << "Could not load all images. Please ensure Res folder is populated" << endl;
        exit( 1 );
    }
    
    units.CreateUnit( PLY, (e_unitDir)( (rand() % 7 + 1) ), getScreenW() / 2, getScreenH()/2 );
    
}
 
void Yukkuri::Think( const int& iElapsedTime )
{
    // Do time-based calculations
    if ( player_movex != 0 or player_movey != 0)
        units.GetPlayer()->moveUnit( player_movex, player_movey );
}
 
void Yukkuri::Render( SDL_Surface* pDestSurface )
{
     // Display slick graphics on screen
    for (int i = 0; i <= (units.GetUnitVecSize() - 1); i++) {
        ApplySurface( units.GetUnit(i)->getUnitX(), units.GetUnit(i)->getUnitY(), img, pDestSurface, NULL);        
    }    
}
 
void Yukkuri::KeyDown(const int& iKeyEnum)
{
    
    switch (iKeyEnum)
    {
        case SDLK_LEFT:
            // Left arrow pressed
            player_movex = -1;
            break;
        case SDLK_RIGHT:
            // Right arrow pressed
            player_movex = 1;
            break;
        case SDLK_UP:
            // Up arrow pressed
            player_movey = -1;
            break;
        case SDLK_DOWN:
            // Down arrow pressed
            player_movey = 1;
            break;
    }
}
 
 
void Yukkuri::KeyUp(const int& iKeyEnum)
{
    switch (iKeyEnum)
    {
    case SDLK_LEFT:
      // Left arrow released
      player_movex = 0;
      break;
    case SDLK_RIGHT:
      // Right arrow released
      player_movex = 0;
      break;
    case SDLK_UP:
      // Up arrow released
      player_movey = 0;
      break;
    case SDLK_DOWN:
      // Down arrow released
      player_movey = 0;
      break;
    }
}
 
void Yukkuri::MouseMoved(const int& iButton, 
               const int& iX, 
               const int& iY, 
               const int& iRelX, 
               const int& iRelY)
{
    // Handle mouse movement
 
    // iX and iY are absolute screen positions
    // iRelX and iRelY are screen position relative to last detected mouse movement
}
 
void Yukkuri::MouseButtonUp(const int& iButton, 
                  const int& iX, 
                  const int& iY, 
                  const int& iRelX, 
                  const int& iRelY)
{
    // Handle mouse button released
}
 
void Yukkuri::MouseButtonDown(const int& iButton, 
                const int& iX, 
                const int& iY, 
                const int& iRelX, 
                const int& iRelY)
{
    // Handle mouse button pressed
}
 
void Yukkuri::WindowInactive()
{
    // Pause game
}
 
void Yukkuri::WindowActive()
{
    // Un-pause game
}
 
 
void Yukkuri::End()
{
    // Clean up
}