/*
 * Spawner.cpp
 *
 *  Created on: 26.08.2010
 */
#include "Spawner.h"
#include "config.h"

extern MainConfig conf;

void Spawner::Spawn( )
{
	float x, y;
	if( UnitManager::units.GetUnitsSize(ENTITY) < conf.maxSpawn ){
		x = y = 0;
		getPosition( &x, &y );
		DynamicUnit* u = dynamic_cast<DynamicUnit*>( UnitManager::units.CreateUnit( ENTITY, x, y ) );
		DynamicUnit* p = dynamic_cast<DynamicUnit*>( UnitManager::units.GetPlayer( ) );
		if( u && p )
			u->levelUp( rand( ) % static_cast<int>(p->getUnitParameter("level")) + 3 );
	}
	if( UnitManager::units.GetUnitsSize(PLANT) < conf.maxEdibles ){
		x = y = 0;
		getPosition( &x, &y );
		UnitManager::units.CreateUnit( PLANT, x, y );
	}
}

void Spawner::getPosition( float* retx, float* rety )
{
	(*retx) = UnitManager::units.GetPlayer()->getUnitX() +
			( ( rand( ) % ( conf.windowWidth / 2) ) + conf.windowWidth / 2 ) * ( ( rand()%2 ) ? 1 : -1 );
	(*rety) = UnitManager::units.GetPlayer()->getUnitY() +
			( ( rand( ) % ( conf.windowHeight / 2) ) + conf.windowHeight / 2 ) * ( ( rand()%2 ) ? 1 : -1 );
}
