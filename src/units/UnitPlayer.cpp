
#include "UnitPlayer.h"
//#include "config.h"


//extern MainConfig conf;

Player::Player()
{
	force.x = 0;
	force.y = 0;
}


/*
void Player::attack( )
{
	Unit* victim = NULL;
	victim = scope.closest( utEntity, 120.0 );
	if( victim )
		this->attackUnit( victim );
}
*/

/*
void Player::eat( )
{
	Unit* victim = NULL;
	victim = scope.closest( FoodTypes, 300 );
	if( victim )
		UnitDynamic::eat( victim );
}
*/

/*
void Player::die( )
{
	if( conf.playerDies ){
		UnitDynamic::die( );
	}
}
*/

bool Player::calculateForce( ){
	if(force == cpvzero && physBody->v == cpvzero ){
		clearMoving();
	}else
		setMoving();
	return isMoving();
}

void Player::moveUnit( short axis, signed int val )
{
	if( axis )
		force.x = val;
	else
		force.y = val;
	calculateForce();
}

