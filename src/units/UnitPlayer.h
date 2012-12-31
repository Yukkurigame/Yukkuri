#ifndef PLAYER_H
#define PLAYER_H

#include "UnitDynamic.h"

class Player : public UnitDynamic
{
public:
	Player();

	//void attack( );
	//void eat( );
	//void die( );

	void moveUnit( short axis, signed int val );
	bool calculateForce( );


protected:
	virtual CUData* createUData();

};

#endif //PLAYER_H
