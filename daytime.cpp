#include "daytime.h"
#include "Interface.h"
#include "unitmanager.h"
#include "config.h"

extern MainConfig conf;

DayTime::DayTime()
{
	Day = 1;
	time = 10.0;
	sfield = NULL;
}

DayTime::~DayTime()
{
	RenderManager::Instance()->FreeGLSprite( sfield );
}

void DayTime::loadInterface()
{
	sfield = RenderManager::Instance()->CreateGLSprite(0, 0, 4, conf.windowWidth, conf.windowHeight );
	sfield->clr->set( 0, 0, 0, 0 );
	text = UI::yui.GetWidget("time");
}

void DayTime::update( const int& dt )
{
	float hours = 24.0f * dt / (conf.dayLength * 10000.0f);
	time = fmod( time + hours , 24.0f );
	if( time > 22 || time < 2){
		if( dark != 128 )
			dark = 128;
		if ( time < 2 ){
			if( !Updated ){
				Day++;
				UnitManager::units.grow();
				Updated = true;
			}
		}else{
			if( Updated )
				Updated = false;
		}
		if(text)
			text->setText("Midnight");
	}else if( time > 18 ){
		float p = (time - 18) / 4.0f;
		dark = static_cast<int>(128 * p);
		if(time > 20.0){
			if(text)
				text->setText("Twilight");
		}else{
			if(text)
				text->setText("Evening");
		}
	}else if( time < 6 ){
		float p = 1 - (time - 2) / 4.0f;
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

void DayTime::onDraw( ){
	if( ldark != dark && sfield ){
		sfield->clr->a = dark;
	}
	ldark = dark;
}
