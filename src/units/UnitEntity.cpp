
#include "units/UnitEntity.h"
#include "units/UnitCorpse.h"
#include "units/unitmanager.h"


Entity::Entity()
{
	//target = NULL;
}


Entity::~Entity( )
{
	//while( nextTarget() ) continue;
}


bool Entity::update( const Frame& frame )
{
	switch( frame.command ){
		case acMove:
			move( );
			break;
		/*case acEAddPathTarget:
			addTarget( );
			break;
		*/
		default:
			return UnitDynamic::update( frame );
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

/*
void Entity::takeAction( )
{
	UnitDynamic::takeAction( );
	if( Attacked && !Attacked->isDeleted() ){
		if( dist(Attacked) < getUnitSize( ) * 100 ){
			attackUnit( Attacked );
		}
	}

}
*/

/*	Add random target to path */
/*
void Entity::addTarget( )
{
	float x = getUnitX() + ( -150 + ( rand() % 300 ) );
	float y = getUnitY() - ( -150 + ( rand() % 300 ) );
	addTarget( x, y );
}
*/

/*
void Entity::move( )
{
	if( waytarget == NULL && !nextTarget() ){
		Image.setFrame(0);
		clearMoving();
		if( physBody )
			cpBodySetVel( physBody, cpvzero );
		return;
	}
	force.x = 0;
	force.y = 0;
	if( !moveUnit(waytarget->x, waytarget->y) )
		nextTarget();
}
*/
