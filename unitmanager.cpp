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
    Unit *temp = new Unit;
    
    temp->setUnitName("reimu");
    //cout << "create unit image" << endl;
    temp->setUnitImage( Graphics::graph.LoadImage( temp->getUnitName( ) ) );
    //cout << "load unit animation" << endl;
    Graphics::graph.LoadAnimation( temp->getUnitName( ), 11, 4, 79, 68 );    
    //cout << "success" << endl;
    
    temp->setUnitType( um_ID );
    temp->setUnitPos( x, y );
    
    AddUnit( temp );
    
    if(um_ID == PLAYER)
        um_player = temp;
    
    return;
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

SDL_Rect* UnitManager::getAnim( Unit* unit )
{     
    return Graphics::graph.GetAnim( unit->getUnitName( ), unit->getUnitAnim( ) );
}


void UnitManager::DrawUnits( SDL_Surface* pDestSurface, const float camX, const float camY )
{
    for (int i = 0; i < (int)m_vUnits.size(); i++) {
        Graphics::graph.ApplySurface( m_vUnits[i]->getUnitX() - camX, m_vUnits[i]->getUnitY() - camY, 
                                                           m_vUnits[i]->getUnitImage(), pDestSurface, getAnim( m_vUnits[i] ) );
        
    }
}