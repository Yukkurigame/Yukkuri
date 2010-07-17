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
			delete Units.back();
		}
		Units.pop_back();
	}
}
/**
 * Build a unit according to criteria passed to the function and call
 * AddUnit to push it onto the stack
 **/
void UnitManager::CreateUnit( enum e_unitID um_ID, int x, int y )
{
	Unit *temp;
	switch(um_ID){
		case 1:
			temp = new Player();
			break;
		case 2:
			temp = new Entity();
			break;
		default:
			temp = new Unit();
			break;
	}

	if( !temp->Create() ||
		!temp->setUnitImage( graph->LoadGLTexture( temp->getUnitImageName( ) ) ) ||
		!temp->loadAnimation()
	)
	{
		delete temp;
		return;
	}

	//cout << "load unit animation" << endl;
	graph->LoadAnimation( temp->getUnitName(), temp->getUnitImageRows(),
								temp->getUnitImageCols(), temp->getUnitWidth(), temp->getUnitHeight(),
								temp->getUnitImage()->w, temp->getUnitImage()->h);
	//cout << "success" << endl;

	temp->setUnitType( um_ID );
	temp->setUnitPos( x, y );

	AddUnit( temp );

	if(um_ID == PLAYER){
		//temp->setPlayer(true);
		player = temp;
	}

	return;
}

void UnitManager::tick( const int& dt )
{
	for (int i = 0; i < (int)Units.size(); i++) {
		Units[i]->update( dt );
	}
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

void UnitManager::DrawUnits( const float camX, const float camY )
{
	Unit* u = NULL;
	for (int i = 0; i < (int)Units.size(); i++) {
		u = Units[i];
		graph->DrawGLTexture( u->getUnitImage( ),
				graph->GetVertex( u->getUnitX( ) - camX, u->getUnitY( ) - camY, 0.0,
											u->getUnitWidth( ), u->getUnitHeight( ), 1 ),
				graph->GetAnimation( u->getUnitName( ), u->getUnitAnim( ) ), &u->getUnitImage( )->clr
			);
	}
}
