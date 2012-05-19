#ifndef PLAYER_H
#define PLAYER_H

#include "Dynamic.h"

class Player : public DynamicUnit
{
public:
	Player();

	void Die( );

	void moveUnit( short axis, signed int val );
	void update( const int& );

protected:
	virtual CUData* createUData();

private:
	signed int moveX;
	signed int moveY;
};

#endif //PLAYER_H
