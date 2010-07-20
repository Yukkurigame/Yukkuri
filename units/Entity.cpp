#include "Entity.h"

Entity::Entity()
{
	gotoX = 0.0;
	gotoY = 0.0;
	ForceGo = false;
	Attacked = NULL;
}

void Entity::Die( )
{
	this->Delete();
}

void Entity::update( const int& dt )
{
	if( stat.hp <= 0 ){
		Die( );
		return;
	}
	AnimatedUnit::update( dt );
	if( Attacked ){
		float dst = dist(Attacked);
		if( stat.hp * 3 <= stat.hpMax && dst < 500 ){ //Run away
			signed int px = (( Attacked->getUnitX() > this->X ) ? -1 : 1);
			signed int py = (( Attacked->getUnitY() > this->Y ) ? -1 : 1);
			setPathTarget( this->X + 500 * px, this->Y + 500 * py );
		}else if( dst  > Scale * 100 ){ //Get closer
			setPathTarget( Attacked->getUnitX(), Attacked->getUnitY() );
		}
	}
	move( dt );
}

void Entity::takeAction( )
{
	DynamicUnit::takeAction( );
	if( Attacked ){
		if( dist(Attacked) < Scale * 100 ){
			attackUnit( Attacked );
		}
	}else{
		if( !isMoved() ){
			float x = getUnitX() + ( -150 + ( rand() % 300 ) );
			float y = getUnitY() - ( -150 + ( rand() % 300 ) );
			setPathTarget(x, y);
		}
	}
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
