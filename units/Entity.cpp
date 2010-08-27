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

void Entity::Die( )
{
	Corpse* corpse;
	corpse = dynamic_cast<Corpse*>( UnitManager::units.CreateUnit( CORPSE, getUnitX(), getUnitY() ) );
	if( corpse ){
		vector<int> bcolor;
		getConfigValue( "bloodcolor", bcolor );
		if( bcolor.size() >= 3 )
			corpse->setBloodColor( bcolor[0], bcolor[1], bcolor[2] );
		else if( bcolor.size() >= 1 )
			corpse->setBloodColor( bcolor[0] );
		corpse->setHP( this->stat.hpMax * this->stat.fed / 2 );
		corpse->setUnitSize( this->getUnitSize( ) );
	}
	if( this->Attacked ){
		this->Attacked->expChange( this->stat.hpMax / this->stat.level );
		this->Attacked->killsChange(1);
	}
	this->Delete();
}

void Entity::update( const int& dt )
{
	if( stat.hp <= 0 ){
		Die( );
		return;
	}
	DynamicUnit::update( dt );
	if( Attacked ){
		float dst = dist(Attacked);
		if( stat.hp * 3 <= stat.hpMax && dst < 500 ){ //Run away
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
	if( Attacked ){
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
