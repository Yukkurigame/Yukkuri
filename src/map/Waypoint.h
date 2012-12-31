#ifndef WAYPOINT_H
#define WAYPOINT_H


#include "basic_types.h"


struct Waypoint
{
	s2f position;
	int critical;
	Waypoint() : position(), critical(0) {};
	Waypoint( float X, float Y, int crit ) : position(X, Y), critical(crit) {};
};


#endif //WAYPOINT_H
