#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include <vector>
using std::vector;

#include "unit.h"
#include "Graphics.h"

class UnitManager
{
    public:
        UnitManager();
        virtual ~UnitManager();
        
        void DrawUnits( SDL_Surface* pDestSurface, const float camX, const float camY );
        void CreateUnit( enum e_unitID um_ID, int x, int y );
        Unit* GetUnit( unsigned int iIndex );
        Unit* GetPlayer() { return um_player; }

        int GetUnitVecSize() { return (int)m_vUnits.size();}

    private:        
        void AddUnit( Unit* pUnit );
        SDL_Rect* getAnim( Unit* );
        vector< Unit* > m_vUnits;
        Unit* um_player;        
};

#endif // UNITMANAGER_H 