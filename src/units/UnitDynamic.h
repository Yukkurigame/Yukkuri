#ifndef DYNAMIC_UNIT_H
#define DYNAMIC_UNIT_H

#include "Unit.h"
#include "basic_types.h"
#include "units/Scope.h"
#include "map/Waypoint.h"


void call_updateAnimOnMovement(cpBody* body, cpFloat dt);



class UnitDynamic: public Unit
{
public:
	UnitDynamic();
	~UnitDynamic();

	//void eat( Unit* Victim );

	virtual bool Create( int id, std::string proto );
	//virtual void die( );

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

	//void takeAction( );

	//Unit* Attacker( ) { return Attacked; }
	//void Attacker( Unit* a ) { Attacked = a; }
	//void attackUnit( Unit* victim );
	//void hit( float damage );

	void move( );
	// Flags for moving
	inline unsigned char isMoving()	{ return flags & ufMoving; }
	inline void setMoving()			{ flags |= ufMoving; }
	inline void clearMoving()		{ flags &= ~ufMoving; }


	/*	Add new random target to path
	void addTarget( ); */
	/*	Add new target to path */
	void addTarget( float x, float y, int critical = 0 );
	inline bool nextTarget( ){
		if( waytarget != NULL )
			delete waytarget;
		if( !path.head ){
			waytarget = NULL;
			return false;
		}
		waytarget = path.head->data;
		path.remove(path.head);
		return true;
	}

	int addPathTarget( lua_State* L );
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
	list< Waypoint* > path;
	Waypoint* waytarget;
};

#endif //DYNAMIC_UNIT_H
