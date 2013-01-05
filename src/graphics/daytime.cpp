
#include "graphics/gl_extensions.h"


#include "graphics/Render.h"
#include "graphics/utils/gl_shader.h"
#include "interface/Interface.h"
#include "interface/widgets/WidgetText.h"
//#include "units/unitmanager.h"
#include "3rdparty/timer/TimerManager.h"
#include "3rdparty/timer/InternalTimerEvent.h"

#include "basic_types.h"
#include "daytime.h"
#include "config.h"

#include <math.h>

#define RADIANS ( M_PI / 180.0 )
#define DAY_PERIOD 200
#define SUN_POSITION 10
#define SUN_HEIGHT 30
#define LATITUDE 10.0
#define LONGITUDE 130.0
#define GTM_DELTA 11
#define LTSM ( LONGITUDE - 15.0 * GTM_DELTA )
/*
#define BASE_NIGHT 128
#define MOON_MOD 64
*/

extern MainConfig conf;

struct light {
	GLfloat position[4];
	GLfloat color[4];
};



namespace{

	WidgetText*	text;
	int Day;
	float Time;
	bool Updated;
	light LightSun;

	struct ProcessShader {
		GLint shader_id;
		GLint sun_position;
		GLint sun_color;
		ProcessShader( GLint id ){
			this->shader_id = id;
			// Initialize
			for( int i = 0; i < 4; ++i )
				LightSun.color[i] = 1.0;

			this->sun_position = glGetUniformLocation( this->shader_id, "lights.position" );
			this->sun_color = glGetUniformLocation( this->shader_id, "lights.color" );
		}
		void setUp( ){
			glUseProgram( this->shader_id );
			glUniform4fv( this->sun_position, 1, &LightSun.position[0] );
			glUniform4fv( this->sun_color, 1, &LightSun.color[0] );
			glUseProgram( 0 );
		}
	};

	list <ProcessShader*> light_shaders;


	class Caller : public ITimerEventPerformer
	{
	public:
		virtual ~Caller(){ };

		void Start(){
			Timer::AddInternalTimerEvent( this, 1, DAY_PERIOD, 0, true, true );
		}

		void OnTimer( InternalTimerEvent& ev ){
			DayTime::update( ev.period );
		}

		void OnTimerEventDestroy( const InternalTimerEvent& ev ){	}
	};
	Caller DayTimer;
}


void calculate_sunpos( float time, GLfloat sun[4] )
{
	// Realistic, lol: http://pveducation.org/pvcdrom/properties-of-sunlight/suns-position
	float B = (360.0 / 365.0) * ((Day % 365) - 81.0) * RADIANS;
	float EoT = 9.87 * sin(2.0*B) - 7.53 * cos(B) - 1.5 * sin(B);
	float TC = 4.0 * LTSM + EoT;
	float LST = time + TC / 60.0;
	float HRA = 15.0 * RADIANS * ( LST - 12.0 );
	float delta = 23.45 * RADIANS * sin(B);

	float Elevation = asin(
			sin(delta) * sin(LATITUDE * RADIANS) +
			cos(delta) * cos(LATITUDE * RADIANS) * cos(HRA));
	float Azimuth = acos(
			( sin(delta) * cos(LATITUDE * RADIANS) - cos(delta) * sin(LATITUDE * RADIANS) * cos(HRA) ) /
			cos(Elevation));
	if( HRA > 0 )
		Azimuth = 2 * M_PI - Azimuth;
	sun[0] = SUN_POSITION * cos(Elevation) * sin(Azimuth);
	sun[1] = SUN_POSITION * sin(Elevation) * sin(Azimuth);
	sun[2] = SUN_HEIGHT * sin(Elevation);
	sun[3] = 0.0;
}



void DayTime::init()
{
	Day = 1;
	Time = 10.0;
	memset( &LightSun.position[0], 0, (unsigned)sizeof(GLfloat) * 4 );
	addShader( Shaders::getProgram( "lighting" ) );
}

void DayTime::clean()
{
	listElement<ProcessShader*>* t = light_shaders.head;
	while( t != NULL ){
		delete t->data;
		t = t->next;
	}
	light_shaders.clear();
}

bool DayTime::addShader( GLint id )
{
	// Check if already in list
	listElement<ProcessShader*>* head = light_shaders.head;
	while( head != NULL ){
		if( head->data->shader_id == id )
			return false;
		head = head->next;
	}
	// Add new
	light_shaders.push( new ProcessShader( id ) );
	return true;
}


bool DayTime::removeShader( GLint id )
{
	// Check if shader in list
	listElement<ProcessShader*>* head = light_shaders.head;
	ProcessShader* s = NULL;
	while( head != NULL ){
		if( head->data->shader_id == id ){
			s = head->data;
			break;
		}
		head = head->next;
	}
	if( s == NULL )
		return false;
	light_shaders.remove( s );
	return true;
}


void DayTime::loadInterface()
{
	text = reinterpret_cast<WidgetText*>(Interface::GetWidget( "time", NULL ));
	DayTimer.Start();
}

void DayTime::update( const UINT& dt )
{
	float hours = 24.0f * dt / (conf.dayLength * 10000.0f);
	Time = fmod( Time + hours , 24.0f );
	if( Time > 22 || Time < 2){
		if ( Time < 2 ){
			if( !Updated ){
				Day++;
				Updated = true;
			}
		}else{
			if( Updated )
				Updated = false;
		}
		if(text)
			text->setWidgetText("Midnight");
	}else if( Time > 18 ){
		if(text)
			text->setWidgetText("Evening");
		/*if(Time > 20.0){
			if(text)
				text->setText("Twilight");
		}else{
			if(text)
				text->setText("Evening");
		}*/
	}else if( Time < 6 ){
		if(text)
			text->setWidgetText("Night");
		/*if( Time > 4 ){
			if(text)
				text->setText("Dawn");
		}else{
			if(text)
				text->setText("Night");
		}*/
	}else{
		if( Time < 11 ){
			if(text)
				text->setWidgetText("Morning");
		}else if( Time > 14 ){
			if(text)
				text->setWidgetText("Afternoon");
		}else{
			if(text)
				text->setWidgetText("Noon");
		}
	}


	calculate_sunpos( Time, &(LightSun.position)[0] );

	listElement<ProcessShader*>* head = light_shaders.head;
	while( head != NULL ){
		head->data->setUp();
		head = head->next;
	}
}


int DayTime::getDay( )
{
	return Day;
}
