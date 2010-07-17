#ifndef DYNAMIC_UNIT_H
#define DYNAMIC_UNIT_H

#include "unit.h"
#include "stdlib.h"
#include <map>
#include <vector>

struct stats{
	float hp;
	float hpMax;
	float fed;
	int exp;
	int expMax;
	float damage;
	int days;
	float scale;
	int level;
	float speed;
	stats( ){
		hp = hpMax = 10; fed = 1; exp = 20; expMax = 200; damage = 3; level = 1;
		scale = 0.3; days = 0; speed = 80;
	}
};

class DynamicUnit: public Unit
{
public:
	DynamicUnit();
	void moveUnit(  signed int x, signed int y , const int& dt);
	void grow();

	void setFirstFrame( );
	//void levelUp


protected:
	AnimationDefs Animdef;
	bool loadAnimation();
    float TotalDistance;
    const static int m_animdistance = 20;
    stats stat;


private:
    int* anim;
};

#endif //DYNAMIC_UNIT_H
