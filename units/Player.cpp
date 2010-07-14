#include "Player.h"

Player::Player()
{
	moveX = 0;
	moveY = 0;
}

void Player::moveUnit( short axis, signed int val )
{
	if( axis )
		moveX = val;
	else
		moveY = val;
}

void Player::update( const int& dt )
{
	DynamicUnit::moveUnit( moveX, moveY, dt );
}

