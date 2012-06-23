#ifndef DYNAMIC_UNIT_H
#define DYNAMIC_UNIT_H

#include "Animated.h"
#include "map/Map.h"
#include <map>
#include <vector>

class DynamicUnit: public AnimatedUnit
{
public:
	DynamicUnit();
	void moveUnit(  signed int x, signed int y , const int& dt);
	void eat( );
	void eat( Unit* Victim );

	virtual bool Create( int id );
	virtual void die( );

	virtual void levelUp( int addlevel );

	void takeAction( );
	void grow( );

	Unit* Attacker( ) { return Attacked; }
	void Attacker( Unit* a ) { Attacked = a; }
	void attack( );
	void attackUnit( Unit* victim );
	void hit( float damage );

protected:
	virtual CUData* createUData();

	std::vector< std::string > FoodTypes;
	Unit* Attacked;
	float TotalDistance;
	const static int m_animdistance = 20;

private:
	int currentTile;
};

#endif //DYNAMIC_UNIT_H
