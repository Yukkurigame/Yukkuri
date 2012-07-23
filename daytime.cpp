
#include "graphics/gl_extensions.h"

#include "daytime.h"
#include "Render.h"
#include "graphics/gl_shader.h"
#include "Interface.h"
#include "unitmanager.h"
#include "config.h"
#include "3rdparty/timer/TimerManager.h"
#include "3rdparty/timer/InternalTimerEvent.h"

#include <math.h>


#define DAY_PERIOD 200
#define BASE_NIGHT 128
#define MOON_MOD 64

extern MainConfig conf;

namespace{

	Widget*	text;
	int Day;
	float Time;
	bool Updated;
	Sprite* sfield;
	GLint colorLight = -1;
	UINT night = BASE_NIGHT;
	//TODO: Можно делать красивые штуки, как в питоновской версии, вот только нужно ли?
	//Каждому углу свою прозрачность и свой цвет.

	class Caller : public ITimerEventPerformer
	{
	public:
		virtual ~Caller(){ };

		void Start(){
			Timer::AddInternalTimerEvent( this, 1, DAY_PERIOD, 0, true, false );
		}

		void OnTimer( InternalTimerEvent& ev ){
			DayTime::update( ev.period );
		}

		void OnTimerEventDestroy( const InternalTimerEvent& ev ){	}
	};
	Caller DayTimer;
}


void DayTime::init()
{
	Day = 1;
	Time = 10.0;
	sfield = NULL;
}

void DayTime::clean()
{
	RenderManager::FreeGLSprite( sfield );
}

void DayTime::loadInterface()
{
	sfield = RenderManager::CreateGLSprite(0, 0, 4, conf.windowWidth, conf.windowHeight );
	sfield->setFixed();
	sfield->clr.set( 0, 0, 0, 0 );
	// Load daytime shader and get uniform vars.
	sfield->shader = Shaders::getProgram( "daytime" );
	colorLight = glGetUniformLocation( sfield->shader, "colorLight" );
	glUseProgram( sfield->shader );
	glUniform4f( colorLight, 1.0, 1.0, 0.0, 0.0 );
	glUseProgram( 0 );
	text = Interface::GetWidget( "time", NULL );
	DayTimer.Start();
}

void DayTime::update( const UINT& dt )
{
	float hours = 24.0f * dt / (conf.dayLength * 10000.0f);
	Time = fmod( Time + hours , 24.0f );
	if( Time > 22 || Time < 2){
		if( sfield->clr.a != night )
			sfield->clr.a = night;
		if ( Time < 2 ){
			if( !Updated ){
				Day++;
				night = BASE_NIGHT + MOON_MOD * ( ( Day % 29 ) / 29 );
				UnitManager::grow();
				Updated = true;
			}
		}else{
			if( Updated )
				Updated = false;
		}
		if(text)
			text->setText("Midnight");
	}else if( Time > 18 ){
		float p = (Time - 18) / 4.0f;
		sfield->clr.a = static_cast<int>( night * p );
		//sfield->clr.a = std::min( night, static_cast<UINT>( 255 * p ) );
		if(Time > 20.0){
			if(text)
				text->setText("Twilight");
		}else{
			if(text)
				text->setText("Evening");
		}
	}else if( Time < 6 ){
		float p = 1 - (Time - 2) / 4.0f;
		sfield->clr.a = std::min( night , static_cast<UINT>( 256 * p ) );
		//sfield->clr.a = static_cast<int>( night * p );
		if( Time > 4 ){
			if(text)
				text->setText("Dawn");
		}else{
			if(text)
				text->setText("Night");
		}
	}else{
		if( sfield->clr.a != 0 )
			sfield->clr.a = 0;
		if( Time < 11 ){
			if(text)
				text->setText("Morning");
		}else if( Time > 14 ){
			if(text)
				text->setText("Afternoon");
		}else{
			if(text)
				text->setText("Noon");
		}
	}

	glUseProgram( sfield->shader );
	if( Time > 4 && Time < 8 ){
		float p = (Time - 4.0f) / 4.0f;
		glUniform4f( colorLight, p, p, 0.0f, 0.25f * (float)sin( M_PI * ( 1.0f - p )) );
	}else{
		glUniform4f( colorLight, 0.0f, 0.0f, 0.0f, 0.0f );
	}
	glUseProgram( 0 );
}


int DayTime::getDay( )
{
	return Day;
}
