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

	void eat( Unit* Victim );

	virtual bool Create( int id, std::string proto );
	virtual void die( );

	void update( const int& dt );
	virtual bool update( const Frame& f );
	void updateAnimOnMovement( cpBody* body, cpFloat dt );

	bool moveUnit( signed int x, signed int y );
	virtual bool calculateForce( );
	void applyForce( );

	inline bool isScope(){ return vis != NULL; }
	void setScope();
	void clearScope();

	void takeAction( );

	Unit* closest( enum unitType type, float limit = 100.0 );
	Unit* closest( list< enum unitType >* types, float limit = 100.0 );

	Unit* Attacker( ) { return Attacked; }
	void Attacker( Unit* a ) { Attacked = a; }
	void attackUnit( Unit* victim );
	void hit( float damage );

	// Flags for moving
	inline unsigned char isMoving()	{ return flags & ufMoving; }
	inline void setMoving()			{ flags |= ufMoving; }
	inline void clearMoving()		{ flags &= ~ufMoving; }

	list< Unit* > Collisions;

protected:
	virtual CUData* createUData();

	cpShape* scopeShape;
	cpVect force;
	Sprite* vis;
	s2i target;

	list< enum unitType > FoodTypes;
	Unit* Attacked;
	cpFloat TotalDistance;
	const static int m_animdistance = 20;

private:
	int currentTile;
};

#endif //DYNAMIC_UNIT_H
