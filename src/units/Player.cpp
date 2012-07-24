
#include "Player.h"
#include "config.h"


extern MainConfig conf;

Player::Player()
{
	force.x = 0;
	force.y = 0;
}

void Player::die( )
{
	if( conf.playerDies ){
		DynamicUnit::die( );
	}
}

void Player::moveUnit( short axis, signed int val )
{
	if( axis )
		force.x = phys.mass / 2 * val;
	else
		force.y = phys.mass / 2 * val;
	cpBodySetVel( physBody, cpvzero );
	cpBodyApplyImpulse( physBody, force, cpvzero );
	if( force.x == 0 && force.y == 0 )
		Image.setDefault();
}

bool Player::update( const Frame& frame )
{
	switch( frame.command ){
		case acMove:
			//DynamicUnit::moveUnit( moveX, moveY );
			break;
		default:
			return DynamicUnit::update( frame );
			break;
	}
	return true;
}
