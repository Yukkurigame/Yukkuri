/*
 * Plant.cpp
 *
 *  Created on: 18.07.2010
 */
#include "Plant.h"

Plant::Plant( )
{

}

bool Plant::Create( int id )
{
	float nutritive;
	float hp;
	if( !Unit::Create( id ) )
		return false;
	getConfigValue( "nutritive", nutritive );
	getConfigValue( "hp", hp );
	Parameters["nutritive"] = nutritive;
	Parameters["hp"] = hp;
	return true;
}

void Plant::update( const int& dt )
{
	AnimatedUnit::update( dt );
	if( Parameters["hp"] <= 0 ){
		Delete( );
	}
}

void Plant::hit( float damage )
{
	Parameters["hp"] -= damage;
}
