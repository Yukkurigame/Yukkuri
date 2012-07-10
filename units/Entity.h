#ifndef ENTITY_H
#define ENTITY_H

#include "Dynamic.h"


class Entity: public DynamicUnit
{
public:
	Entity( );
	void setPathTarget(float, float);
	bool update( const Frame& f );
	void takeAction( );
	void move( );
	void stopMove(){ ForceGo = false; };
	void startMove(){ ForceGo = true; };
	bool isMoved(){ return ForceGo; };

protected:
	virtual CUData* createUData();

private:
	float gotoX, gotoY;
	bool ForceGo;

};

#endif //ENTITY_H
