#include "Player.h"
#include "Interface.h"

Player::Player()
{
	moveX = 0;
	moveY = 0;
	char d[4];
	sprintf( d, "%d", stat.days );
	UI::yui.GetWidget( "pdays" )->setText( d );
	sprintf( d, "%d", stat.level );
	UI::yui.GetWidget( "plevel" )->setText( d );
	UI::yui.GetWidget( "phpbar" )->setBarSize( stat.hpMax );
	UI::yui.GetWidget( "pexpbar" )->setBarSize( stat.expMax );
	UI::yui.GetWidget( "pfedbar" )->setBarSize( 100 );
}

void Player::moveUnit( short axis, signed int val )
{
	if( axis )
		moveX = val;
	else
		moveY = val;
	if( moveX == 0 and moveY == 0 )
		setFirstFrame( );
}

void Player::update( const int& dt )
{
	DynamicUnit::moveUnit( moveX, moveY, dt );
	UI::yui.GetWidget( "phpbar" )->setBarValue( stat.hp );
	UI::yui.GetWidget( "pexpbar" )->setBarValue( stat.exp );
	UI::yui.GetWidget( "pfedbar" )->setBarValue( stat.fed * 100 );
}

void Player::grow( )
{
	DynamicUnit::grow( );
	char d[15];
	sprintf( d, "%d", stat.days );
	UI::yui.GetWidget( "pdays" )->setText( d );
}
