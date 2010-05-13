#ifndef DYNAMIC_UNIT_H
#define DYNAMIC_UNIT_H

#include "unit.h"

class DynamicUnit: public Unit
{
public:
    void moveUnit(  signed int x, signed int y , const int& dt);

protected:
    float m_fdistance;

};

#endif //DYNAMIC_UNIT_H