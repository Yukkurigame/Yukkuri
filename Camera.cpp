#include "Camera.h"
#include "config.h"
#include "Graphics.h"

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
	posX -= x;
	posY -= y;
	Graphics::Instance()->MoveGlScene( x , y, 0 );
	/*switch( TargetMode ){
		case TARGET_MODE_CENTER:
			//Graphics::Instance()->MoveGlScene( -1 , -1, 0 );
			//Graphics::Instance()->MoveGlScene( -x + conf.windowWidth / 2 , -y + conf.windowHeight / 2, 0 );
			break;
		default:
			//Graphics::Instance()->MoveGlScene( -x, -y, 0 );
			break;
	}*/

}

void YCamera::ChangeMode( int mode )
{
	if( mode == TargetMode )
		return;
	TargetMode = mode;
	//FIXME: Remove old mode before add new
	switch( mode ){
		case TARGET_MODE_CENTER:
			Graphics::Instance()->MoveGlScene( conf.windowWidth / 2, conf.windowHeight / 2, 0 );
	}
}

void YCamera::SetTarget( float* x, float* y )
{
	TargetX = x;
	TargetY = y;
	ChangeMode(TARGET_MODE_CENTER);
	Update( );
}
