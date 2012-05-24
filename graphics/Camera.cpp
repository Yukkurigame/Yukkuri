

#include "graphics/Camera.h"
#include "units/Unit.h"
#include "config.h"
#include <math.h>


extern MainConfig conf;


namespace {
	int TargetMode = ctmNormal;
	s2f pos;
	s2f offset;
	float* TargetX = NULL;
	float* TargetY = NULL;
	Unit* Target = NULL;
	s3f* MainView = NULL;
}



void Camera::init( s3f* view )
{
	MainView = view;
}


void Camera::Update( )
{
	MainView->x = -floor( pos.x - offset.x );
	MainView->y = -floor( pos.y - offset.y );
	if( TargetX && TargetY ){
		if( (*TargetX) != pos.x || (*TargetY) != pos.y ){
			Move( pos.x - (*TargetX), pos.y - (*TargetY) );
		}
	}
}


float Camera::GetX( )
{
	if( TargetX != NULL ){
		if( TargetMode == ctmCenter ){
			return *TargetX - ( conf.windowWidth / 2 );
		}
		return *TargetX;
	}
	return pos.x;
}


float Camera::GetY( )
{
	if( TargetY != NULL ){
		if( TargetMode == ctmCenter ){
			return *TargetY - ( conf.windowHeight / 2 );
		}
		return *TargetY;
	}
	return pos.y;
}


void Camera::Move( float x, float y )
{
	float dx, dy;
	dx = dy = 0;
	pos.x -= x;
	pos.y -= y;
}


void Camera::ChangeMode( enum ctMode mode )
{
	if( mode == TargetMode )
		return;
	TargetMode = mode;
	switch( mode ){
		case ctmCenter:
			offset.x = conf.windowWidth >> 1;
			offset.y = conf.windowHeight >> 1;
			break;
		case ctmNormal:
			offset.x = offset.y = 0;
			break;
	}
}


void Camera::SetTarget( Unit* u )
{
	DeleteTarget( );
	if( !u )
		return;
	Target = u;
	SetTarget( u->getUnitpX(), u->getUnitpY() );
}


void Camera::SetTarget( float* x, float* y )
{
	TargetX = x;
	TargetY = y;
	ChangeMode( ctmCenter );
	Update( );
}


Unit* Camera::GetTarget( )
{
	return Target;
}


void Camera::DeleteTarget( )
{
	Target = NULL;
	TargetX = NULL;
	TargetY = NULL;
	ChangeMode( ctmNormal );
}
