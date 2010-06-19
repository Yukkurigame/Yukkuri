#ifndef DYNAMIC_UNIT_H
#define DYNAMIC_UNIT_H

#include "unit.h"
#include <map>
#include <vector>

class DynamicUnit: public Unit
{
public:
	DynamicUnit();
	void moveUnit(  signed int x, signed int y , const int& dt);


protected:
	std::map < string, std::vector <int> > AnimTmp;
	bool loadAnimation();
    float m_fdistance;
    const static int m_animdistance = 20;

private:
    //TODO: change in to int[2]
    std::vector <int> anim;

};

#endif //DYNAMIC_UNIT_H
