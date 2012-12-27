#ifndef DYNAMIC_UNIT_H
#define DYNAMIC_UNIT_H

#include "Unit.h"
#include "basic_types.h"
#include "units/Scope.h"


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
	void applyForce( const int& dt );

	inline bool isScope(){ return scope.sprite && scope.sprite->isVisible(); }
	inline void setScope() { scope.set(); }
	inline void clearScope() { scope.clear(); };
	inline void addToScope( Unit* u ) {  scope.add( u ); }
	inline void removeFromScope( Unit* u ) {  scope.remove( u ); }

	void takeAction( );

	//Unit* Attacker( ) { return Attacked; }
	//void Attacker( Unit* a ) { Attacked = a; }
	//void attackUnit( Unit* victim );
	//void hit( float damage );

	// Flags for moving
	inline unsigned char isMoving()	{ return flags & ufMoving; }
	inline void setMoving()			{ flags |= ufMoving; }
	inline void clearMoving()		{ flags &= ~ufMoving; }


	int getClosest( lua_State* L );


protected:
	virtual CUData* createUData();

	Scope scope;

	cpVect force;
	s2i target;

	int FoodTypes;
	//Unit* Attacked;
	cpFloat TotalDistance;
	const static int m_animdistance = 20;

private:
	int currentTile;
};

#endif //DYNAMIC_UNIT_H
