#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include <vector>
using std::vector;

#include "unit.h"

class UnitManager
{
    public:
        UnitManager();
        virtual ~UnitManager();

        void CreateUnit( enum e_unitID um_ID, enum e_unitDir um_Dir, int x, int y );
        Unit* GetUnit( unsigned int iIndex );
        Unit* GetPlayer() { return um_player; }

        int GetUnitVecSize() { return (int)m_vUnits.size();}

    protected:
        void AddUnit( Unit* pUnit );
        vector< Unit* > m_vUnits;
        Unit* um_player;
};

#endif // UNITMANAGER_H 