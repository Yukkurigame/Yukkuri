#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include <map>
#include <vector>
#include <string>

#include "Unit.h"
#include "Render.h"

class UnitManager
{
	public:
		UnitManager();
		virtual ~UnitManager();

		static UnitManager units;

		Unit* CreateUnit( enum unitType type, float x, float y );
		void DeleteUnit( Unit* u );
		void BatchRemove( );
		Unit* GetUnit( unsigned int id );
		Unit* GetPlayer() { return player; }
		int GetUnitsSize( enum unitType type );
		int GetUnitVecSize() { return (int)Units.size();}

		void tick( const int& );

		Unit* closer( Unit* u, std::string type, float limit = 100.0 );
		Unit* closer( Unit* u, std::vector< std::string >* types, float limit = 100.0 );
		void grow( ); //FUUUUU

		void onDraw( );


	private:
		RenderManager* graph;
		void AddUnit( Unit* pUnit );
		void ChangeUnitsSize( enum unitType type, signed int size );
		std::map< unsigned int, Unit* > Units;
		std::vector< Unit* > RemovedUnits;
		std::map< enum unitType, int > Size;
		Unit* player;
};

#endif // UNITMANAGER_H
