#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "SDL/SDL_image.h"
#include "SDL_opengl.h"
#include "Define.h"
#include "yloader.h"
#include <map>
using std::map;
#include <vector>
using std::vector;
#include <string>
using std::string;

class Graphics
{
    public:
        ~Graphics();
        static Graphics graph;
        SDL_Surface* LoadImage( std::string );
        void ApplySurface( double, double, SDL_Surface*, SDL_Surface* , SDL_Rect* );
        void LoadAnimation( string name, int rows, int cols, int width, int height );        
        SDL_Rect* GetAnim( string name,  int num );
        
    private:        
        void AddImage( SDL_Surface* , string );
        SDL_Surface* GetImage( string );
        SDL_Rect GetSDLRect( int startx, int starty, int width, int height );
        
        map < string,  vector<SDL_Rect> > Animations;
        map < string, SDL_Surface* > LoadedImages;        
        
};


#endif // GRAPHICS_H
