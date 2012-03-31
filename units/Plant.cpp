/*
 * Plant.cpp
 *
 *  Created on: 18.07.2010
 */
#include "Plant.h"
#include "Luaconfig.h"

Plant::Plant( )
{

}

bool Plant::Create( int id )
{
	if( !Unit::Create( id ) )
		return false;

	LuaConfig* cfg = new LuaConfig;
	cfg->getValue( "nutritive", UnitName, Type, Parameters["nutritive"] );
	cfg->getValue( "hp", UnitName, Type, Parameters["hp"] );
	delete cfg;

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
