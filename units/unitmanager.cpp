#include "unitmanager.h"

UnitManager::UnitManager()
{

}

UnitManager::~UnitManager()
{
	while( m_vUnits.size() > 0 )
	{
		if ( m_vUnits.back() != 0 ) {
			delete m_vUnits.back();
		}
		m_vUnits.pop_back();
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
		!temp->setUnitImage( Graphics::graph.LoadGLTexture( temp->getUnitImageName( ) ) ) ||
		//!temp->setUnitSprite( Graphics::graph.CreateGLSprite( 0.0, 0.0, 0.0, 0.0, 0.0,
								//temp->getUnitWidth(), temp->getUnitHeight(),
								//Graphics::graph.LoadGLTexture( temp->getUnitImageName( ) ), 0 )
							//)// ||
		!temp->loadAnimation()
	)
	{
		delete temp;
		return;
	}

	//cout << "load unit animation" << endl;
	Graphics::graph.LoadAnimation( temp->getUnitName(), temp->getUnitImageRows(),
								temp->getUnitImageCols(), temp->getUnitWidth(), temp->getUnitHeight(),
								temp->getUnitImage()->w, temp->getUnitImage()->h);
	//cout << "success" << endl;

	temp->setUnitType( um_ID );
	temp->setUnitPos( x, y );

	AddUnit( temp );

	if(um_ID == PLAYER)
		temp->setPlayer(true);
		um_player = temp;

	return;
}

void UnitManager::tick( const int& dt )
{
	for (int i = 0; i < (int)m_vUnits.size(); i++) {
		m_vUnits[i]->update( dt );
	}
}

void UnitManager::AddUnit( Unit* pUnit )
{
	m_vUnits.push_back( pUnit );
}

Unit* UnitManager::GetUnit( unsigned int iIndex )
{
	if ( iIndex < 0 || iIndex > m_vUnits.size() )
		return 0;

	return m_vUnits.at( iIndex );
}

coord2farr* UnitManager::getAnim( Unit* unit )
{
	return Graphics::graph.GetAnimation( unit->getUnitName( ), unit->getUnitAnim( ) );
}


void UnitManager::DrawUnits( const float camX, const float camY )
{
	for (int i = 0; i < (int)m_vUnits.size(); i++) {
		Unit* u = m_vUnits[i];
		Graphics::graph.DrawGLTexture( u->getUnitImage( ),
				Graphics::graph.GetVertex( u->getUnitX( ) - camX, u->getUnitY( ) - camY, 0.0,
											u->getUnitWidth( ), u->getUnitHeight( ) ),
				Graphics::graph.GetAnimation( u->getUnitName( ), u->getUnitAnim( ) )
				);

		//Graphics::graph.GetCoordinates( 0.0, 0.0, u->getUnitWidth(), u->getUnitHeight(),
										//u->getUnitImage()->w, u->getUnitImage()->h, 0 )
		//Graphics::graph.CreateGLSprite( u->getUnitX() - camX, u->getUnitY() - camY, 0.0, 0.0, 0.0,
										//u->getUnitWidth(), u->getUnitHeight(), u->getUnitImage(), 0 )
		//Graphics::graph.DrawGLTexture(u->getUnitX() - camX, u->getUnitY() - camY, u->getUnitImage(),
										//u->getUnitWidth(), u->getUnitHeight() );
		//Graphics::graph.ApplySurface( m_vUnits[i]->getUnitX() - camX, m_vUnits[i]->getUnitY() - camY,
										//m_vUnits[i]->getUnitImage(), pDestSurface, getAnim( m_vUnits[i] ) );

	}
}
