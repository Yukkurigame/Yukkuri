#ifndef PLAYER_H
#define PLAYER_H

#include "UnitDynamic.h"

class Player : public UnitDynamic
{
public:
	Player();

	void die( );

	void moveUnit( short axis, signed int val );


protected:
	virtual CUData* createUData();

};

#endif //PLAYER_H
