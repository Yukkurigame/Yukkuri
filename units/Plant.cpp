/*
 * Plant.cpp
 *
 *  Created on: 18.07.2010
 */
#include "Plant.h"

Plant::Plant( )
{
	nutritive = 0.0;
	hp = 1;
}

bool Plant::Create( )
{
	if( !Unit::Create( ) )
		return false;
	getConfigValue("nutritive", nutritive);
	getConfigValue("hp", hp);
	return true;
}

void Plant::update( const int& dt )
{
	AnimatedUnit::update( dt );
	if( hp <= 0 ){
		Delete( );
	}
}

void Plant::hit( float damage )
{
	hp -= damage;
}
