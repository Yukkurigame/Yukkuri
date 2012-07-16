
#include "unitmanager.h"
#include "Plant.h"
#include "Corpse.h"
#include "Entity.h"
#include "Player.h"

#include "graphics/Camera.h"
#include "scripts/Lua.h"


#include "debug.h"
#include "hacks.h"

namespace {
	static unsigned int LastId = 1;

	std::map< unsigned int, Unit* > Units;
	std::vector< Unit* > RemovedUnits;
	std::map< enum unitType, int > Size;
	Unit* player = NULL;

	void AddUnit( Unit* unit ) {
		Units[ unit->getUnitId() ] = unit;
	}

	void ChangeUnitsSize( enum unitType type, signed int size ) {
		if( Size.count(type) < 1 )
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
		ChangeUnitsSize( u->getUniteType( ), -1 );
		delete u;
	}

}


void UnitManager::init()
{
	player = NULL;
}

void UnitManager::clean()
{
	Debug::debug( Debug::UNIT, "Remove units.\n" );
	FOREACHIT( Units ){
		DeleteUnit( it->second );
	}
	Units.clear();
}
/**
 * Build a unit according to criteria passed to the function and call
 * AddUnit to push it onto the stack
 **/
Unit* UnitManager::CreateUnit( enum unitType type, float x, float y, const char* proto )
{
	Unit* temp;
	switch(type){
		case utPlayer:
			temp = new Player();
			break;
		case utEntity:
			temp = new Entity();
			break;
		case utPlant:
			temp = new Plant();
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
		protoname = std::string(proto);

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
	RemovedUnits.push_back( u );
}


void UnitManager::BatchRemove()
{
	if ( !RemovedUnits.size() )
		return;

	FOREACHIT( RemovedUnits ){
		Units.erase( (*it)->getUnitId() );
		DeleteUnit( *it );
	}

	RemovedUnits.clear();
}


Unit* UnitManager::GetPlayer()
{
	return player;
}


int UnitManager::GetUnitsSize( enum unitType type )
{
	if( Size.count(type) > 0 )
		return Size[type];
	return 0;
}


int UnitManager::GetUnitVecSize()
{
	return (int)Units.size();
}


Unit* UnitManager::closer( Unit* u, std::string type, float limit )
{
	//FIXME: quick and dirty
	if( u == NULL )
		return NULL;
	Unit* ret = NULL;
	float distance = 9000;
	limit *= u->getUnitSize();
	Unit* tmp = NULL;
	FOREACHIT( Units ){
		tmp = it->second;
		if( tmp != NULL && tmp != u && tmp->getUnitType() == type ){
			float dist = u->dist( tmp );
			if( dist < limit && dist < distance ){
				distance = dist;
				ret = tmp;
			}
		}
	}

	return ret;
}


Unit* UnitManager::closer( Unit* u, std::vector< std::string >* types, float limit )
{
	//FIXME: quick and dirty
	if( u == NULL )
		return NULL;
	Unit* ret = NULL;
	float distance = 9000;
	limit *= u->getUnitSize();
	Unit* tmp = NULL;
	FOREACHIT( Units ){
		tmp = it->second;
		if( tmp != NULL && tmp != u ){
			FOREACHP( tit, types ){
				if( tmp->getUnitType() == (*tit) ){
					float dist = u->dist( tmp );
					if( dist < limit && dist < distance ){
						distance = dist;
						ret = tmp;
					}
				}
			}
		}
	}
	return ret;
}


void UnitManager::tick( const int& dt )
{
	Unit* u;
	FOREACHIT( Units ){
		u = it->second;
		if( u->isDeleted() )
			RemovedUnits.push_back( u );
		else
			u->update( dt );
	}
	BatchRemove();
}


void UnitManager::grow( )
{
	FOREACHIT( Units ){
		it->second->grow();
	}
}


Unit* UnitManager::GetUnit( unsigned int id )
{
	Unit* u = NULL;
	std::map< UINT, Unit* >::iterator it = Units.find( id );
	if( it != Units.end() )
		u = it->second;
	return u;
}
