#include "Entity.h"

Entity::Entity()
{
	gotoX = 0.0;
	gotoY = 0.0;
	force_go = false;
}


void Entity::update( const int& dt )
{
	if( !isMoved() ){
		float x = getUnitX() + ( -150 + ( rand() % 300 ) );
		float y = getUnitY() - ( -150 + ( rand() % 300 ) );
		setPathTarget(x, y);
	}
	move( dt );
}

void Entity::move( const int& dt )
{
	if( !isMoved() ){
		setFirstFrame();
		return;
	}
	int dx = 0, dy = 0;
	int nx = static_cast<int>(gotoX - getUnitX());
	int ny = static_cast<int>(gotoY - getUnitY());
	if( abs(nx) > 1 )
		dx = nx / abs(nx);
	if( abs(ny) > 1 )
		dy = ny / abs(ny);
	if( dx == 0 && dy == 0){
		stopMove();
		return;
	}
	moveUnit( dx, dy, dt );
}

void Entity::setPathTarget(float X, float Y)
{
	if( isMoved() )
		return;
	gotoX = X;
	gotoY = Y;
	startMove();
}
