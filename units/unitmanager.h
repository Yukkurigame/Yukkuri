#ifndef UNITMANAGER_H
#define UNITMANAGER_H

#include <map>
#include <vector>
#include <string>

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

	Unit* closer( Unit* u, std::string type, float limit = 100.0 );
	Unit* closer( Unit* u, std::vector< std::string >* types, float limit = 100.0 );
	void grow( ); //FUUUUU



};

#endif // UNITMANAGER_H
