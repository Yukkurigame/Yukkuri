#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include <map>
#include <vector>
using std::vector;

#include "unit.h"
#include "Plant.h"
#include "Corpse.h"
#include "Entity.h"
#include "Player.h"
#include "Render.h"

class UnitManager
{
	public:
		UnitManager();
		virtual ~UnitManager();

		static UnitManager units;

		Unit* CreateUnit( enum unitType type, float x, float y );
		void DeleteUnit( Unit* u );
		Unit* GetUnit( unsigned int id );
		Unit* GetPlayer() { return player; }
		int GetUnitsSize( enum unitType type );
		int GetUnitVecSize() { return (int)Units.size();}

		void tick( const int& );

		Unit* closer( Unit* u, string type, float limit = 100.0 );
		Unit* closer( Unit* u, vector< string >* types, float limit = 100.0 );
		void grow( ); //FUUUUU

		void onDraw( );


	private:
		RenderManager* graph;
		void AddUnit( Unit* pUnit );
		void ChangeUnitsSize( enum unitType type, signed int size );
		vector< Unit* > Units;
		std::map< enum unitType, int > Size;
		Unit* player;
};

#endif // UNITMANAGER_H
