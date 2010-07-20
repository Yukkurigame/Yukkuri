#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include <vector>
using std::vector;

#include "unit.h"
#include "Plant.h"
#include "Entity.h"
#include "Player.h"
#include "Graphics.h"

class UnitManager
{
    public:
        UnitManager();
        virtual ~UnitManager();

        static UnitManager units;

        void DrawUnits( const float camX, const float camY );
        void CreateUnit( enum e_unitID um_ID, int x, int y );
        Unit* GetUnit( unsigned int Index );
        Unit* GetPlayer() { return player; }

        void tick( const int& );

        Unit* closer( Unit* u, string type, float limit = 100.0 );
        void grow( ); //FUUUUU

        int GetUnitVecSize() { return (int)Units.size();}

    private:
        Graphics* graph;
        void AddUnit( Unit* pUnit );
        coord2farr* getAnim( Unit* );
        vector< Unit* > Units;
        Unit* player;
};

#endif // UNITMANAGER_H
