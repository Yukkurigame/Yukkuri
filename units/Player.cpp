#include "Player.h"
#include "Interface.h"


Player::Player()
{
	moveX = 0;
	moveY = 0;

	FoodTypes.push_back( "corpse" );
}

void Player::moveUnit( short axis, signed int val )
{
	if( axis )
		moveX = val;
	else
		moveY = val;
	if( moveX == 0 && moveY == 0 )
		setFirstFrame( );
}

void Player::update( const int& dt )
{
	DynamicUnit::update( dt );
	DynamicUnit::moveUnit( moveX, moveY, dt );
}

void Player::toggleInterface( )
{
	//FIXME: static
	Widget* w;
	w =UI::yui.GetWidget( "pstats" );
	if( w )
		w->toggleVisibility( );
}
