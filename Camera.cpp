#include "Camera.h"
#include "config.h"
#include "Graphics.h"
#include <math.h>

extern MainConfig conf;

YCamera YCamera::CameraControl;

YCamera::YCamera( )
{
	posX = posY = 0.0f;
	TargetX = TargetY = NULL;
	TargetMode = TARGET_MODE_NORMAL;
}

void YCamera::Update( )
{
	Graphics::Instance()->MoveGlScene( -floor( posX - offsetX ) , -floor( posY - offsetY ), 0 );
	if( TargetX && TargetY ){
		if( (*TargetX) != posX || (*TargetY) != posY ){
			Move( posX - (*TargetX), posY - (*TargetY) );
		}
	}
}

float YCamera::GetX( )
{
	if( TargetX != NULL ){
		if( TargetMode == TARGET_MODE_CENTER ){
			return *TargetX - ( conf.windowWidth / 2 );
		}
		return *TargetX;
	}
	return posX;
}

float YCamera::GetY( )
{
	if( TargetY != NULL ){
		if( TargetMode == TARGET_MODE_CENTER ){
			return *TargetY - ( conf.windowHeight / 2 );
		}
		return *TargetY;
	}
	return posY;
}

void YCamera::Move( float x, float y )
{
	float dx, dy;
	dx = dy = 0;
	posX -= x;
	posY -= y;
}

void YCamera::ChangeMode( int mode )
{
	if( mode == TargetMode )
		return;
	TargetMode = mode;
	switch( mode ){
		case TARGET_MODE_CENTER:
			offsetX = conf.windowWidth >> 1;
			offsetY = conf.windowHeight >> 1;
			break;
		case TARGET_MODE_NORMAL:
			offsetX = offsetY = 0;
			break;
	}
}

void YCamera::SetTarget( float* x, float* y )
{
	TargetX = x;
	TargetY = y;
	ChangeMode(TARGET_MODE_CENTER);
	Update( );
}
