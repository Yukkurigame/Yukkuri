#ifndef DYNAMIC_UNIT_H
#define DYNAMIC_UNIT_H

#include "Animated.h"
#include <map>
#include <vector>

struct stats{
	float hp;
	float hpMax;
	float fed;
	float exp;
	float expMax;
	float damage;
	int level;
	float speed;
	int days;
	int kills;
	stats( ){ hp = hpMax = 10; fed = 1; exp = 20; expMax = 200; damage = 3; level = 1; speed = 80; days = kills = 0; }
};

class DynamicUnit: public AnimatedUnit
{
public:
	DynamicUnit();
	stats* getUnitStats( ) { return &stat; }
	void moveUnit(  signed int x, signed int y , const int& dt);
	void grow();
	void eat( Unit* Victim );
	virtual void levelUp( int addlevel );

	int getLevel( ) { return stat.level; }

	void update( const int& dt );
	void takeAction( );

	void expChange( float exp ) { stat.exp += exp; }
	void killsChange( int k ) { stat.kills += k; }

	DynamicUnit* Attacker( ) { return Attacked; }
	void Attacker( DynamicUnit* a ) { Attacked = a; }
	void attackUnit( Unit* victim );
	void hit( float damage );

	void setFirstFrame(  );


protected:
	std::vector< string > FoodTypes;
	AnimationDefs Animdef;
	bool loadAnimation();
	DynamicUnit* Attacked;
	float TotalDistance;
	const static int m_animdistance = 20;
	stats stat;


private:
	int* anim;
};

#endif //DYNAMIC_UNIT_H
