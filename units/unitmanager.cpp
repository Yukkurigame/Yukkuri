#include "unitmanager.h"
#include "Plant.h"
#include "Corpse.h"
#include "Entity.h"
#include "Player.h"

#include "hacks.h"

UnitManager UnitManager::units;

static unsigned int LastId = 1;

UnitManager::UnitManager()
{
	graph = RenderManager::Instance();
	player = NULL;
}

UnitManager::~UnitManager()
{
	while( Units.size() > 0 )
	{
		if ( Units.back() != 0 ) {
			DeleteUnit( Units.back() );
		}
		Units.pop_back();
	}
}
/**
 * Build a unit according to criteria passed to the function and call
 * AddUnit to push it onto the stack
 **/
Unit* UnitManager::CreateUnit( enum unitType type, float x, float y )
{
	Unit* temp;
	switch(type){
		case PLAYER:
			temp = new Player();
			break;
		case ENTITY:
			temp = new Entity();
			break;
		case PLANT:
			temp = new Plant();
			break;
		case CORPSE:
			temp = new Corpse();
			break;
		default:
			temp = new Unit();
			break;
	}

	temp->setUnitType( type );

	if( !temp->Create( LastId ) ){
		delete temp;
		return NULL;
	}

	temp->setUnitPos( x, y );

	AddUnit( temp );
	ChangeUnitsSize( type, 1 );

	LastId++;

	if( type == PLAYER ){
		DeleteUnit( player );
		player = temp;
	}

	return temp;
}

void UnitManager::DeleteUnit( Unit* u )
{
	if( !u )
		return;
	if( u == player )
		player = NULL;
	ChangeUnitsSize( u->geteUnitType( ), -1 );
	delete u;
}

void UnitManager::BatchRemove()
{
	if (!RemovedUnits.size())
		return;

	FOREACHIT( RemovedUnits ){
		DeleteUnit( *(*it) );
		Units.erase( *it );
	}

	RemovedUnits.clear();
}

int UnitManager::GetUnitsSize( enum unitType type )
{
	if( Size.count(type) > 0 )
		return Size[type];
	return 0;
}

Unit* UnitManager::closer( Unit* u, std::string type, float limit )
{
	//FIXME: quick and dirty
	Unit* ret = NULL;
	float distance = 9000;
	limit *= u->getUnitSize();
	for (int i = 0; i < (int)Units.size(); i++) {
		if( Units[i] != u && Units[i]->getUnitType() == type ){
			float dist = u->dist(Units[i]);
			if( dist < limit && dist < distance ){
				distance = dist;
				ret = Units[i];
			}
		}
	}
	return ret;
}

Unit* UnitManager::closer( Unit* u, std::vector< std::string >* types, float limit )
{
	//FIXME: quick and dirty
	Unit* ret = NULL;
	float distance = 9000;
	limit *= u->getUnitSize();
	for (int i = 0; i < (int)Units.size(); i++) {
		if( Units[i] != u ){
			for( std::vector< std::string >::iterator it = types->begin(), end = types->end(); it != end; ++it ){
				if( Units[i]->getUnitType() == (*it) ){
					float dist = u->dist(Units[i]);
					if( dist < limit && dist < distance ){
						distance = dist;
						ret = Units[i];
					}
					break;
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
		u = *it;
		u->update( dt );
		if( u->isDeleted() )
			RemovedUnits.push_back( it );
	}
	BatchRemove();
}

void UnitManager::grow( )
{
	for (int i = 0; i < (int)Units.size(); i++) {
		Units[i]->grow( );
	}
}


Unit* UnitManager::GetUnit( unsigned int id )
{
	Unit* u = NULL;
	if( id > 0 && id <= LastId ){
		for( std::vector< Unit* >::iterator it = Units.begin(), end = Units.end(); it != end; ++it ){
			if( (*it)->getUnitId( ) == id ){
				u = (*it);
			}
		}
	}
	return u;
}

void UnitManager::onDraw( )
{
	/*Unit* u = NULL;
	for( std::vector< Unit* >::iterator it = Units.begin(), end = Units.end(); it != end; ++it ){
		if( (*it) ){
			(*it)->onDraw();
		}
	}*/
}

void UnitManager::AddUnit( Unit* unit )
{
	Units.push_back( unit );
}

void UnitManager::ChangeUnitsSize( enum unitType type, signed int size )
{
	if( Size.count(type) < 1 )
		Size[type] = 0;
	Size[type] += size;
	if( Size[type] < 0 )
		Size[type] = 0;
}
