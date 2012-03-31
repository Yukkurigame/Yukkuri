#include "Entity.h"
#include "Corpse.h"
#include "unitmanager.h"

Entity::Entity()
{
	gotoX = 0.0;
	gotoY = 0.0;
	ForceGo = false;
	Attacked = NULL;
}

void Entity::update( const int& dt )
{
	DynamicUnit::update( dt );
	if( this->isDeleted() )
		return;
	if( Attacked && !Attacked->isDeleted() ){
		float dst = dist(Attacked);
		if( getUnitParameter( "hp" ) * 3 <= getUnitParameter( "hpmax" ) && dst < 500 ){ //Run away
			signed int px = (( Attacked->getUnitX() > this->X ) ? -1 : 1);
			signed int py = (( Attacked->getUnitY() > this->Y ) ? -1 : 1);
			setPathTarget( this->X + 500 * px, this->Y + 500 * py );
		}else if( dst  > getUnitSize( ) * 100 ){ //Get closer
			setPathTarget( Attacked->getUnitX(), Attacked->getUnitY() );
		}
	}
	move( dt );
}

void Entity::takeAction( )
{
	DynamicUnit::takeAction( );
	if( Attacked && !Attacked->isDeleted() ){
		if( dist(Attacked) < getUnitSize( ) * 100 ){
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
		Image.setFrame(0);
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
