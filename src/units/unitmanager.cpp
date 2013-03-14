#include "units/unitmanager.h"
#include "units/UnitStatic.h"
#include "units/UnitCorpse.h"
#include "units/UnitEntity.h"
#include "units/UnitPlayer.h"

#include "units/physics/physics.h"
#include "graphics/Camera.h"
#include "scripts/Lua.h"

#include "debug.h"
#include "hacks.h"

#include <map>
#include <vector>

namespace
{
	static unsigned int LastId = 1;

	list< Unit* > Units;
	list< Unit* > RemovedUnits;
	std::map< enum unitType, int > Size;
	Unit* player = NULL;

	void AddUnit( Unit* unit )
	{
		Units.push_back( unit );
	}

	void ChangeUnitsSize( enum unitType type, signed int size )
	{
		if( Size.count( type ) < 1 )
			Size[type] = 0;
		Size[type] += size;
		if( Size[type] < 0 )
			Size[type] = 0;
	}

	void DeleteUnit( Unit* u )
	{
		if( !u )
			return;
		if( u == player )
			player = NULL;
		if( u == Camera::GetTarget() )
			Camera::DeleteTarget();
		ChangeUnitsSize( u->getUnitType(), -1 );
		delete u;
	}

}

void UnitManager::init( )
{
	player = NULL;
	Phys::init();
	//cpSpaceUseSpatialHash( Phys::space, 300.0, 3000.0 );
}

void UnitManager::clean( )
{
	Debug::debug( Debug::UNIT, "Remove units.\n" );
	ITER_LIST( Unit*, Units ) {
		DeleteUnit( it->data );
	}
	Units.clear();
	Phys::clean();
}
/**
 * Build a unit according to criteria passed to the function and call
 * AddUnit to push it onto the stack
 **/
Unit* UnitManager::CreateUnit( enum unitType type, float x, float y, const char* proto )
{
	Unit* temp;
	switch( type ){
		case utPlayer:
			temp = new Player();
			break;
		case utEntity:
			temp = new Entity();
			break;
		case utPlant:
			temp = new UnitStatic();
			break;
		case utCorpse:
			temp = new Corpse();
			break;
		default:
			temp = new Unit();
			break;
	}

	temp->setUnitType( type );

	std::string protoname = "";
	if( proto != NULL )
		protoname = std::string( proto );

	if( !temp->Create( LastId, protoname ) ){
		delete temp;
		return NULL;
	}

	temp->setUnitPos( x, y );

	AddUnit( temp );
	ChangeUnitsSize( type, 1 );

	LastId++;

	if( type == utPlayer ){
		RemoveUnit( player );
		player = temp;
	}

	return temp;
}

// Mark unit to removing.
void UnitManager::RemoveUnit( Unit* u )
{
	if( !u )
		return;
	u->setDeleted();
	RemovedUnits.push( u );
}

void UnitManager::BatchRemove( )
{
	if( RemovedUnits.head == NULL )
		return;

	ITER_LIST( Unit*, RemovedUnits ) {
		if( !it->data )
			continue;
		Units.remove( it->data );
		DeleteUnit( it->data );
		it->data = NULL;
	}
	RemovedUnits.clear();
}

Unit* UnitManager::GetPlayer( )
{
	return player;
}

int UnitManager::GetUnitsSize( enum unitType type )
{
	if( Size.count( type ) > 0 )
		return Size[type];
	return 0;
}

int UnitManager::GetUnitVecSize( )
{
	return Units.count;
}

void UnitManager::tick( const int& dt )
{
	ITER_LIST( Unit*, Units ){
		Unit* u = it->data;
		if( u->isDeleted() )
			RemovedUnits.push( u );
		else
			u->update( dt );
	}
	BatchRemove();
}

Unit* UnitManager::GetUnit( unsigned int id )
{
	Unit* u = NULL;
	// TODO: binary search here
	ITER_LIST( Unit*, Units ){
		if( it->data->getUnitId() == id ){
			u = it->data;
			break;
		}
	}
	return u;
}
