#include "Player.h"
#include "Interface.h"
#include "unitmanager.h"


Player::Player()
{
	moveX = 0;
	moveY = 0;
	char d[4];
	sprintf( d, "%d", static_cast<int>(getUnitParameter( "days" )) );
	UI::yui.GetWidget( "pdays" )->setText( d );
	sprintf( d, "%d", static_cast<int>(getUnitParameter( "level" )) );
	//FIXME: static.
	UI::yui.GetWidget( "plevel" )->setText( d );
	UI::yui.GetWidget( "phpbar" )->setBarSize( static_cast<int>( getUnitParameter( "hpmax" ) ) );
	UI::yui.GetWidget( "pexpbar" )->setBarSize( static_cast<int>( getUnitParameter( "expmax" ) ) );
	UI::yui.GetWidget( "pfedbar" )->setBarSize( 100 );

	UI::yui.GetWidget( "phpbar" )->bindValue( getUnitpParameter( "hp" ) );
	UI::yui.GetWidget( "pexpbar" )->bindValue( getUnitpParameter( "exp" ) );
	UI::yui.GetWidget( "pfedbar" )->bindValue( getUnitpParameter( "fed" ) );

	FoodTypes.push_back( "corpse" );
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
	DynamicUnit::update( dt );
	DynamicUnit::moveUnit( moveX, moveY, dt );
	//UI::yui.GetWidget( "phpbar" )->setBarValue( static_cast<int>(stat.hp) );
	//UI::yui.GetWidget( "pexpbar" )->setBarValue( static_cast<int>(stat.exp) );
	//UI::yui.GetWidget( "pfedbar" )->setBarValue( static_cast<int>(stat.fed * 100) );
}

void Player::attack( )
{
	Unit* victim = NULL;
	victim = UnitManager::units.closer( this, "entity", 120.0 );
	if( victim )
		this->attackUnit( victim );
}

void Player::eat( )
{
	Unit* victim = NULL;
	victim = UnitManager::units.closer( this, &FoodTypes );
	if( victim )
		DynamicUnit::eat( victim );
}

void Player::grow( )
{
	DynamicUnit::grow( );
	char d[15];
	sprintf( d, "%d", static_cast<int>(getUnitParameter( "days" )) );
	UI::yui.GetWidget( "pdays" )->setText( d );
}

void Player::levelUp( int addlevel )
{
	DynamicUnit::levelUp( addlevel );
	char d[4];
	sprintf( d, "%d", static_cast<int>(getUnitParameter( "days" )) );
	UI::yui.GetWidget( "pdays" )->setText( d );
	sprintf( d, "%d", static_cast<int>(getUnitParameter( "level" )) );
	//FIXME: static.
	UI::yui.GetWidget( "plevel" )->setText( d );
	UI::yui.GetWidget( "phpbar" )->setBarSize( static_cast<int>(getUnitParameter( "hpmax" )) );
	UI::yui.GetWidget( "pexpbar" )->setBarSize( static_cast<int>(getUnitParameter( "expmax" )) );
	UI::yui.GetWidget( "pfedbar" )->setBarSize( 100 );
}

void Player::toggleInterface( )
{
	UI::yui.GetWidget( "pstats" )->toggleVisibility( );
}
