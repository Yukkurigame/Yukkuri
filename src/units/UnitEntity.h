#ifndef ENTITY_H
#define ENTITY_H

#include "units/UnitDynamic.h"
//#include "map/Waypoint.h"
//#include <queue>


class Entity: public UnitDynamic
{
public:
	Entity( );
	~Entity( );

	bool update( const Frame& f );
	//void takeAction( );
	//void move( );
	/*	Add new random target to path */
	//void addTarget( );
	/*	Add new target to path */
	/*inline void addTarget( float x, float y ){
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
	*/

protected:
	virtual CUData* createUData();

/*
private:
	std::queue < Waypoint* > path;
	Waypoint* target;
*/

};

#endif //ENTITY_H
