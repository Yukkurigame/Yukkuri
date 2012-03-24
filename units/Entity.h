#ifndef ENTITY_H
#define ENTITY_H

#include "Dynamic.h"


class Entity: public DynamicUnit
{
public:
	Entity( );
	void setPathTarget(float, float);
	void update( const int& );
	void takeAction( );
	void move( const int& );
	void stopMove(){ ForceGo = false; };
	void startMove(){ ForceGo = true; };
	bool isMoved(){ return ForceGo; };

private:
	float gotoX, gotoY;
	bool ForceGo;

};

#endif //ENTITY_H
