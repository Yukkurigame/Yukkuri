#include "daytime.h"

DayTime::DayTime()
{
    Day = 1;
    time = 10.0;    
    sfield = SDL_CreateRGBSurface( SDL_SWSURFACE, WWIDTH, WHEIGHT, 24, 0, 0, 0, 0 );
}

DayTime::~DayTime()
{
    SDL_FreeSurface( sfield );
}


void DayTime::update( const int& dt )
{
    float hours = 24.0 * dt / DAY_LENGTH;
    time = fmod( time + hours , 24.0 );
    if( time > 22 or time < 2){
    	if( dark != 128 )
            dark = 128;
        if ( time > 0.0f ){
            if( !days_update ){
                Day++;
                days_update = true;
            }
        }else{
        	if( days_update )
                days_update = false;
        }
    }else if( time > 18 ){
        float p = (time - 18) / 4.0;
        dark = 128 * p;
    }else if( time < 6 ){
    	float p = 1 - (time - 2) / 4.0;        
        dark = std::min( 128 , static_cast<int>( 256 * p ) );
    }else{
    	if( dark != 0 )    	    
           dark = 0;
    }
}

void DayTime::draw( SDL_Surface* pDestSurface ){
	//if( ldark != dark )
        //Graphics::graph.SetAlpha( sfield, dark );
    //Graphics::graph.ApplySurface( 0, 0, sfield, pDestSurface, NULL );
	ldark = dark;
}
