#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include <vector>

#include "Unit.h"


namespace UnitManager
{

	void init();
	void clean();

	Unit* CreateUnit( enum unitType type, float x, float y, const char* proto = NULL );
	void RemoveUnit( Unit* u );
	void BatchRemove( );
	Unit* GetUnit( unsigned int id );
	Unit* GetPlayer();
	int GetUnitsSize( enum unitType type );
	int GetUnitVecSize();

	void tick( const int& );

};

#endif // UNITMANAGER_H
