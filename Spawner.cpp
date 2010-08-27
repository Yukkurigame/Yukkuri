/*
 * Spawner.cpp
 *
 *  Created on: 26.08.2010
 */
#include "Spawner.h"

void Spawner::Spawn( )
{
	float x, y;
	if( UnitManager::units.GetUnitsSize(ENTITY) < MAX_SPAWN ){
		x = y = 0;
		getPosition( &x, &y );
		DynamicUnit* u = dynamic_cast<DynamicUnit*>( UnitManager::units.CreateUnit( ENTITY, x, y ) );
		DynamicUnit* p = dynamic_cast<DynamicUnit*>( UnitManager::units.GetPlayer( ) );
		if( u && p )
			u->levelUp( rand( ) % p->getLevel() + 3 );
	}
	if( UnitManager::units.GetUnitsSize(PLANT) < MAX_EDIBLES ){
		x = y = 0;
		getPosition( &x, &y );
		UnitManager::units.CreateUnit( PLANT, x, y );
	}
}

void Spawner::getPosition( float* retx, float* rety )
{
	(*retx) = UnitManager::units.GetPlayer()->getUnitX() +
			( ( rand( ) % ( WWIDTH / 2) ) + WWIDTH/2 ) * ( ( rand()%2 ) ? 1 : -1 );
	(*rety) = UnitManager::units.GetPlayer()->getUnitY() +
			( ( rand( ) % ( WHEIGHT / 2) ) + WHEIGHT/2 ) * ( ( rand()%2 ) ? 1 : -1 );
}
