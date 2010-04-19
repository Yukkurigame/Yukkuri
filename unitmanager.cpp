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
void UnitManager::CreateUnit( enum e_unitID um_ID, enum e_unitDir um_Dir, int x, int y )
{
    Unit *temp = new Unit;

    temp->setUnitType( um_ID );
    temp->setUnitDir( um_Dir );
    temp->setUnitPos( x, y );
    
    AddUnit( temp );
    
    if(um_ID == PLY)
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
