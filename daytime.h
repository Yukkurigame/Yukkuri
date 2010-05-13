#ifndef TIMEOFDAY_H
#define TIMEOFDAY_H

#include <math.h>
#include <SDL/SDL.h> //for SDL_Rect
#include "Graphics.h"
#include "Define.h"

class DayTime
{
    public:
        DayTime();
        ~DayTime();
        void update( const int& dt );
        void draw( SDL_Surface* pDestSurface );
        int getDay( ){ return Day; }        
        
    private:
        int Day;
        float time;
        bool days_update;
        SDL_Surface* sfield;
        int dark, ldark;        
};

#endif //TIMEOFDAY_H
