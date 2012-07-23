#ifndef PLAYER_H
#define PLAYER_H

#include "Dynamic.h"

class Player : public DynamicUnit
{
public:
	Player();

	void die( );

	void moveUnit( short axis, signed int val );
	bool update( const Frame& f );


protected:
	virtual CUData* createUData();

};

#endif //PLAYER_H
