#ifndef DYNAMIC_UNIT_H
#define DYNAMIC_UNIT_H

#include "Unit.h"
#include "basic_types.h"

#include <vector>


void call_updateAnimOnMovement(cpBody* body, cpFloat dt);



class DynamicUnit: public Unit
{
public:
	DynamicUnit();
	~DynamicUnit();
	void moveUnit( signed int x, signed int y );
	void eat( );
	void eat( Unit* Victim );

	virtual bool Create( int id, std::string proto );
	virtual void die( );

	virtual bool update( const Frame& f );
	virtual void updatePhysics( );
	void updateAnimOnMovement( cpBody* body, cpFloat dt );

	void takeAction( );
	void grow( );

	Unit* Attacker( ) { return Attacked; }
	void Attacker( Unit* a ) { Attacked = a; }
	void attack( );
	void attackUnit( Unit* victim );
	void hit( float damage );

	list< Unit* > Collisions;

protected:
	virtual CUData* createUData();

	cpShape* scopeShape;
	cpVect force;

	std::vector< enum unitType > FoodTypes;
	Unit* Attacked;
	cpFloat TotalDistance;
	const static int m_animdistance = 20;

private:
	int currentTile;
};

#endif //DYNAMIC_UNIT_H
