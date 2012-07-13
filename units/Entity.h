#ifndef ENTITY_H
#define ENTITY_H

#include "Dynamic.h"
#include "map/Waypoint.h"
#include <queue>


class Entity: public DynamicUnit
{
public:
	Entity( );

	bool update( const Frame& f );
	void takeAction( );
	void move( );
	/*	Add new random target to path */
	void addTarget( );
	/*	Add new target to path */
	inline void addTarget( float x, float y ){
		path.push( new Waypoint( x, y ) );
	}
	inline bool nextTarget( ){
		if( target != NULL )
			delete target;
		if( path.empty() ){
			target = NULL;
			return false;
		}
		target = path.front();
		path.pop();
		return true;
	}

	inline unsigned char isMoving()	{ return flags & 4; }
	inline void setMoving()			{ flags |= 4; }
	inline void clearMoving()		{ flags &= ~4; }


protected:
	virtual CUData* createUData();


private:
	std::queue < Waypoint* > path;
	Waypoint* target;

};

#endif //ENTITY_H
