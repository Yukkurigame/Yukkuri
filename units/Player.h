#ifndef PLAYER_H
#define PLAYER_H

#include <math.h>
#include <iostream>
#include <string>
using std::string;
using std::cout;
using std::endl;

#include "Dynamic.h"

class Player : public DynamicUnit
{
public:
    Player();

    void Die( );

    void moveUnit( short axis, signed int val );
    void update( const int& );

    //TODO: Это все вообще бы в луа перенести.
    void toggleInterface( );

private:
    signed int moveX;
    signed int moveY;
};

#endif //PLAYER_H
