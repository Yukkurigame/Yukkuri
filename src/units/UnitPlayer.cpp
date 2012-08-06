
#include "UnitPlayer.h"
#include "config.h"


extern MainConfig conf;

Player::Player()
{
	force.x = 0;
	force.y = 0;
}


void Player::attack( )
{
	Unit* victim = NULL;
	victim = closest( utEntity, 120.0 );
	if( victim )
		this->attackUnit( victim );
}


void Player::eat( )
{
	Unit* victim = NULL;
	victim = closest( &FoodTypes, 300 );
	if( victim )
		UnitDynamic::eat( victim );
}


void Player::die( )
{
	if( conf.playerDies ){
		UnitDynamic::die( );
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

