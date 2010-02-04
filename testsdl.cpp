#include "SDL/SDL.h"
 
int main( int argc, char* args[] ){
    
    SDL_Surface* screen = NULL; 
    
    //Start SDL 
    SDL_Init( SDL_INIT_EVERYTHING );
    
    //add screen
    screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE ); 
    
    //Update Screen 
    SDL_Flip( screen );
    
    //Pause 
    SDL_Delay( 2000 ); 
     
    //Quit SDL 
    SDL_Quit();
    
    return 0; 
}
