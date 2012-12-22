/*
 * Scope.h
 *
 *  Created on: 22.12.2012
 */

#ifndef UNITS_SCOPE_H_
#define UNITS_SCOPE_H_

#include "units/physics/physics.h"
#include "graphics/sprite/Sprite.h"
#include "basic_types.h"


class Unit;


struct Scope {

	Sprite* sprite;
	cpShape* shape;
	s4ub color;

	list< Unit* > Collisions;

	Scope() : sprite(NULL), shape(NULL), color(255, 255, 0, 127) {}
	~Scope();
	bool attach( cpBody* target );
	void update( );
	void set( );
	void clear( );

	inline void add( Unit* u ){ Collisions.push( u ); }
	inline void remove( Unit* u ){ Collisions.remove( u ); }

	Unit* closest( int types, float limit = 100.0 );

};


#endif /* UNITS_SCOPE_H_ */
