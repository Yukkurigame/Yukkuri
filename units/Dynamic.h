#ifndef DYNAMIC_UNIT_H
#define DYNAMIC_UNIT_H

#include "Animated.h"
#include <map>
#include <vector>

class DynamicUnit: public AnimatedUnit
{
public:
	DynamicUnit();
	void moveUnit(  signed int x, signed int y , const int& dt);
	void grow();
	void eat( );
	void eat( Unit* Victim );

	virtual void Die( );

	virtual void levelUp( int addlevel );

	void update( const int& dt );
	void takeAction( );

	Unit* Attacker( ) { return Attacked; }
	void Attacker( Unit* a ) { Attacked = a; }
	void attack( );
	void attackUnit( Unit* victim );
	void hit( float damage );

	void setFirstFrame(  );


protected:
	std::vector< string > FoodTypes;
	AnimationDefs Animdef;
	bool loadAnimation();
	Unit* Attacked;
	float TotalDistance;
	const static int m_animdistance = 20;

private:
	int* anim;
};

#endif //DYNAMIC_UNIT_H
