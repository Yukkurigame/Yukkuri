#ifndef TIMEOFDAY_H
#define TIMEOFDAY_H

#include <math.h>
#include "Graphics.h"
#include "Interface.h"	//FUUUU
#include "Widgets.h"		//UUUUU
#include "Define.h"

class DayTime
{
    public:
        DayTime();
        ~DayTime();
        void update( const int& dt );
        void draw( );
        int getDay( ){ return Day; }
        void loadInterface();
        
    private:
        Widget*	text;

        int Day;
        float time;
        bool days_update;
        Sprite* sfield;
        int dark, ldark;
};

#endif //TIMEOFDAY_H
