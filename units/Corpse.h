/*
 * Corpse.h
 *
 *  Created on: 23.08.2010
 */

#ifndef CORPSE_H_
#define CORPSE_H_

#include "Plant.h"

class Corpse: public Plant
{
public:
	Corpse();
	~Corpse();

	bool Create( int id );

	void update( const int& dt );

	void setBloodColor( int c ) { blood->clr->set( c ); }
	void setBloodColor( int r, int g, int b ) { blood->clr->set( r, g, b ); }

private:
	Sprite* blood;

};

#endif /* CORPSE_H_ */
