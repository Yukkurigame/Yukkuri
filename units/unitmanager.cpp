#include "unitmanager.h"

UnitManager UnitManager::units;

UnitManager::UnitManager()
{
	graph = Graphics::Instance();
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
Unit* UnitManager::CreateUnit( enum e_unitID um_ID, int x, int y )
{
	Unit* temp;
	switch(um_ID){
		case 1:
			temp = new Player();
			break;
		case 2:
			temp = new Entity();
			break;
		case 3:
			temp = new Plant();
			break;
		case 4:
			temp = new Corpse();
			break;
		default:
			temp = new Unit();
			break;
	}

	temp->setUnitType( um_ID );

	if( !temp->Create() ||
		!temp->setUnitImage( graph->CreateGLSprite( temp->getUnitImageName( ) ) ) ||
		!temp->loadAnimation()
	)
	{
		delete temp;
		return NULL;
	}

	graph->LoadAnimation( temp->getUnitName(), temp->getUnitImageRows(),
								temp->getUnitImageCols(), temp->getUnitWidth(), temp->getUnitHeight(),
								temp->getUnitImage()->tex->w, temp->getUnitImage()->tex->h);

	temp->setUnitPos( x, y );
	temp->setUnitSize( 1 );

	AddUnit( temp );

	if(um_ID == PLAYER){
		player = temp;
	}

	return temp;
}

void UnitManager::DeleteUnit( Unit* u )
{
	graph->FreeGLSprite( u->getUnitImage() );
	u->setUnitImage( NULL );
	delete u;
}

void UnitManager::tick( const int& dt )
{
	for( vector< Unit* >::iterator it = Units.begin(), end = Units.end(); it != end; ++it ){
		if( (*it)->isDeleted( ) ){
			DeleteUnit( *it );
			Units.erase( it );
		}else{
			(*it)->update( dt );
		}
	}
}

Unit* UnitManager::closer( Unit* u, string type, float limit )
{
	//FIXME: quick and dirty
	Unit* ret = NULL;
	int distance = 9000;
	for (int i = 0; i < (int)Units.size(); i++) {
		if( Units[i] != u && Units[i]->getUnitType() == type ){
			int dist = u->dist(Units[i]);
			if( dist < limit && dist < distance ){
				distance = dist;
				ret = Units[i];
			}
		}
	}
	return ret;
}

Unit* UnitManager::closer( Unit* u, vector< string >* types, float limit )
{
	//FIXME: quick and dirty
	Unit* ret = NULL;
	int distance = 9000;
	for (int i = 0; i < (int)Units.size(); i++) {
		if( Units[i] != u ){
			for( vector< string >::iterator it = types->begin(), end = types->end(); it != end; ++it ){
				if( Units[i]->getUnitType() == (*it) ){
					int dist = u->dist(Units[i]);
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

void UnitManager::grow( )
{
	for (int i = 0; i < (int)Units.size(); i++) {
		Units[i]->grow( );
	}
}


void UnitManager::AddUnit( Unit* unit )
{
	Units.push_back( unit );
}

Unit* UnitManager::GetUnit( unsigned int index )
{
	if ( index < 0 || index > Units.size() )
		return 0;

	return Units.at( index );
}

coord2farr* UnitManager::getAnim( Unit* unit )
{
	return graph->GetAnimation( unit->getUnitName( ), unit->getUnitAnim( ) );
}

void UnitManager::DrawUnits( )
{
	Unit* u = NULL;
	for (int i = 0; i < (int)Units.size(); i++) {
		u = Units[i];
		u->getUnitImage( )->setPosition( u->getUnitX( ), u->getUnitY( ) );
		u->getUnitImage()->coordinates = graph->GetAnimation( u->getUnitName( ), u->getUnitAnim( ) );
	}
}
