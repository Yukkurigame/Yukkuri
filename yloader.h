#ifndef YUKKURILOADER_H
#define YUKKURILOADER_H

#include <cstdlib>
#include <iostream>
using std::cout;
using std::endl;
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <map>
using std::map;

#include "SDL/SDL_image.h"
#include "SDL_opengl.h" 

class YLoader
{
    public:
        SDL_Surface* LoadImage( std::string );        
        vector< SDL_Rect > animation;
        //map<string, int> wordcounts;
        void LoadAnimation(int rows, int cols, int width, int height);
        SDL_Rect* GetAnim( int num );
        int Init(int);
    private:
        SDL_Rect GetSDLRect( int startx, int starty, int width, int height );
};

#endif //YUKKURILOADER_H