
#include "Entity.h"
#include "Corpse.h"
#include "unitmanager.h"


Entity::Entity()
{
	//gotoX = 0.0;
	//gotoY = 0.0;
	//ForceGo = false;
	target = NULL;
	Attacked = NULL;
}

bool Entity::update( const Frame& frame )
{
	switch( frame.command ){
		case acMove:
			move( );
			break;
		case acEAddPathTarget:
			addTarget( );
			break;
		default:
			return DynamicUnit::update( frame );
			break;
	}
	return true;
	/*if( Attacked && !Attacked->isDeleted() ){
		float dst = dist(Attacked);
		if( Char.get( uStateHP ) * 3 <= Char.get( uParamHP ) && dst < 500 ){ //Run away
			signed int px = (( Attacked->getUnitX() > this->X ) ? -1 : 1);
			signed int py = (( Attacked->getUnitY() > this->Y ) ? -1 : 1);
			setPathTarget( this->X + 500 * px, this->Y + 500 * py );
		}else if( dst  > getUnitSize( ) * 100 ){ //Get closer
			setPathTarget( Attacked->getUnitX(), Attacked->getUnitY() );
		}
	}*/

}

void Entity::takeAction( )
{
	DynamicUnit::takeAction( );
	if( Attacked && !Attacked->isDeleted() ){
		if( dist(Attacked) < getUnitSize( ) * 100 ){
			attackUnit( Attacked );
		}
	}else{
		/*if( !isMoved() ){
			float x = getUnitX() + ( -150 + ( rand() % 300 ) );
			float y = getUnitY() - ( -150 + ( rand() % 300 ) );
			setPathTarget(x, y);
		}*/
	}
}

/*	Add random target to path */
void Entity::addTarget( )
{
	float x = getUnitX() + ( -150 + ( rand() % 300 ) );
	float y = getUnitY() - ( -150 + ( rand() % 300 ) );
	addTarget( x, y );
}

void Entity::move( )
{
	if( target == NULL && !nextTarget() ){
		Image.setFrame(0);
		clearMoving();
		return;
	}
	int dx = 0, dy = 0;
	int nx = static_cast<int>(target->x - getUnitX());
	int ny = static_cast<int>(target->y - getUnitY());
	if( abs(nx) > 1 )
		dx = nx / abs(nx);
	if( abs(ny) > 1 )
		dy = ny / abs(ny);
	if( dx == 0 && dy == 0){
		nextTarget();
		return;
	}
	setMoving();
	moveUnit( dx, dy );

}
