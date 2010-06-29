#ifndef ENTITY_H
#define ENTITY_H

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>
using std::string;
using std::cout;
using std::endl;

#include "Dynamic.h"


class Entity: public DynamicUnit
{
public:
    Entity();
    void setPathTarget(float, float);
    void update( const int& );
    void move( const int& );
    void stopMove(){ force_go = false; };
    void startMove(){ force_go = true; };
    bool isMoved(){ return force_go; };

private:
    float gotoX, gotoY;
    bool force_go;
    
};

#endif //ENTITY_H
