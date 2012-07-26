#ifndef DYNAMIC_UNIT_H
#define DYNAMIC_UNIT_H

#include "Unit.h"
#include "basic_types.h"


void call_updateAnimOnMovement(cpBody* body, cpFloat dt);



class UnitDynamic: public Unit
{
public:
	UnitDynamic();
	~UnitDynamic();
	void moveUnit( signed int x, signed int y );
	void eat( );
	void eat( Unit* Victim );

	virtual bool Create( int id, std::string proto );
	virtual void die( );

	void update( const int& dt );
	virtual bool update( const Frame& f );
	void updateAnimOnMovement( cpBody* body, cpFloat dt );

	inline bool isScope(){ return vis != NULL; }
	void setScope();
	void clearScope();

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
	Sprite* vis;

	list< enum unitType > FoodTypes;
	Unit* Attacked;
	cpFloat TotalDistance;
	const static int m_animdistance = 20;

private:
	int currentTile;
};

#endif //DYNAMIC_UNIT_H
