/*
 * Plant.cpp
 *
 *  Created on: 18.07.2010
 */
#include "Plant.h"


Plant::Plant( )
{

}

void Plant::update( const int& dt )
{
	AnimatedUnit::update( dt );
	if( Parameters["hp"] <= 0 ){
		die( );
	}
}

void Plant::hit( float damage )
{
	Parameters["hp"] -= damage;
}
