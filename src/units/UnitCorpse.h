/*
 * Corpse.h
 *
 *  Created on: 23.08.2010
 */

#ifndef CORPSE_H_
#define CORPSE_H_

#include "UnitStatic.h"

class Corpse: public UnitStatic
{
public:
	Corpse();
	~Corpse();

	bool Create( int id, std::string proto );

	//void setUnitPos( float x, float y );

	void setBloodColor( int c ) {
		//if( blood ) blood->clr.set( c );
	}
	void setBloodColor( int r, int g, int b ) {
		//if( blood ) blood->clr.set( r, g, b );
	}

protected:
	virtual CUData* createUData();

private:
	Sprite* blood;

};

#endif /* CORPSE_H_ */
