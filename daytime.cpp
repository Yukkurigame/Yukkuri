#include "daytime.h"

DayTime::DayTime()
{
	Day = 1;
	time = 10.0;
	sfield = Graphics::graph.CreateGLSprite(0, 0, 0, WWIDTH, WHEIGHT );
	sfield->col->r = 0;
	sfield->col->g = 0;
	sfield->col->b = 0;
	sfield->col->a = 255;
}

DayTime::~DayTime()
{
	Graphics::graph.FreeGLSprite(sfield);
}

void DayTime::loadInterface()
{
	text = UI::yui.GetWidget("time");
}

void DayTime::update( const int& dt )
{
	float hours = 24.0 * dt / (DAY_LENGTH * 10000);
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
		if(text)
			text->setText("Midnight");
	}else if( time > 18 ){
		float p = (time - 18) / 4.0;
		dark = 128 * static_cast<int>(p);
		if(time > 20){
			if(text)
				text->setText("Twilight");
		}else{
			if(text)
				text->setText("Evening");
		}
	}else if( time < 6 ){
		float p = 1 - (time - 2) / 4.0;
		dark = std::min( 128 , static_cast<int>( 256 * p ) );
		if( time > 4 ){
			if(text)
				text->setText("Dawn");
		}else{
			if(text)
				text->setText("Night");
		}
	}else{
		if( dark != 0 )
		   dark = 0;
		if( time < 11 ){
			if(text)
				text->setText("Morning");
		}else if( time > 14 ){
			if(text)
				text->setText("Afternoon");
		}else{
			if(text)
				text->setText("Noon");
		}
	}
}

void DayTime::draw( ){
	if( ldark != dark ){
		sfield->col->a = dark;
	}
	if(dark)
		Graphics::graph.DrawGLTexture( sfield );
	ldark = dark;
}
