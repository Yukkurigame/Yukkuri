#include "Dynamic.h"
#include "Corpse.h"

#include "unitmanager.h"

#include "LuaConfig.h"

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>
#include <math.h>
#include "map.h"



DynamicUnit::DynamicUnit()
{
	TotalDistance = 0;
	Attacked = NULL;

	//FIXME: get it from config
	FoodTypes.push_back( "plant" );
	currentTile = -1;
}

bool DynamicUnit::Create( int id )
{
	if( !AnimatedUnit::Create( id ) )
		return false;

	/*LuaConfig* cfg = new LuaConfig;
	cfg->getValue( "hp", UnitName, Type, Parameters["hp"] );
	cfg->getValue( "damage", UnitName, Type, Parameters["damage"] );
	cfg->getValue( "speed", UnitName, Type, Parameters["speed"]);
	Parameters["hpmax"] = Parameters["hp"];
	Parameters["fed"] = 100;
	Parameters["exp"] = 20;
	Parameters["expmax"] = 200;
	Parameters["level"] = 1;
	Parameters["days"] = 0;*/

	setUnitSize( 0.35f );

	//delete cfg;

	return true;
}



/** Move the unit according to the eight cardinal directions.
	@return Boolean value represents a Collision (true)
	@remark
**/
void DynamicUnit::moveUnit( signed int x, signed int y, const int& dt )
{
	if( x != 0 || y != 0 ){
		//char d[30];
		extern Map map;
		float zone = 1.0;
		float l = sqrt( static_cast<float>(x * x  +  y * y) );
		float speed = fabs( Parameters["speed"] * Parameters["fed"] ) * zone * ( dt / 100000.0f ) / l;
		float dx = speed * x;// / l;
		float dy = speed * y ;// / l;
		/*//MapTile* currentTile = map.GetTile( X , Y );
		if( currentTile && !currentTile->Type->passability ){
			//FIXME: Bad
			int x, y, px, py;
			MapTile* nextTile;
			float df = 9000;
			x = y = 1;
			px = static_cast<int>(X);
			py = static_cast<int>(Y);
			map.toMapCoordinates( &px, &py );
			for( int i = -1; i < 2; ++i ){
				for( int j = -1; j < 2; ++j ){
					if( !i && !j )
						continue;
					nextTile = map.GetTile( px + i, py + j );
					if( !nextTile || !nextTile->Type->passability )
						continue;
					//Если стоять в центре следующего тайла, то не сработает.
					float f = sqrt( pow( static_cast<float>(nextTile->RealX) - X, 2 ) +
									pow( static_cast<float>(nextTile->RealY) - Y, 2 ) );
					if( f < df ){
						df = f;
						x = i;
						y = j;
					}
				}
			}
			currentTile = map.GetTile( px + x, py + y );
			if( currentTile ){
				dx = ( static_cast<float>(currentTile->RealX) - X ) / 4.0f;
				dy = ( static_cast<float>(currentTile->RealY) - Y ) / 4.0f;
				if( dx && dx < 0.01 && dx > -0.01 )
					dx = 0.3f * ( dx < 0 ? -1 : 1 );
				if( dy && dy < 0.01 && dy > -0.01 )
					dy = 0.3f * ( dy < 0 ? -1 : 1 );
			}
		}else{
			currentTile = map.GetTile( X + dx * x, Y + dy * y );
			if( currentTile )
				zone = currentTile->Type->passability;
			if( !zone )
				zone = -0.5;
		}*/
		dx *= zone;
		dy *= zone;
		float distance = sqrt( dx * dx + dy * dy );
		TotalDistance += distance;
		if( dx || dy ) { //FUUU
			if( dx < 0 && dy > 0 )
				Image.setAnimation("leftup");
			else if( dx < 0 && dy < 0 )
				Image.setAnimation("leftdown");
			else if( dx > 0 && dy > 0 )
				Image.setAnimation("rightup");
			else if( dx > 0 && dy < 0 )
				Image.setAnimation("rightdown");
			else if( dy > 0 )
				Image.setAnimation("up");
			else if( dy < 0 )
				Image.setAnimation("down");
			else if( dx < 0 )
				Image.setAnimation("left");
			else if( dx > 0 )
				Image.setAnimation("right");
			Image.setFrame( Image.getCount() ? ( static_cast<int>(TotalDistance) / m_animdistance) % Image.getCount() : 0 );
		}
		X += dx;
		Y += dy;
		Image.setPosition(X, Y);
		if( Parameters["fed"] > 1 )
			Parameters["fed"] -= distance / 200;
	}
}

void DynamicUnit::grow( )
{
	++Parameters["days"];
	Parameters["exp"] += 10 * Parameters["level"];
}

void DynamicUnit::eat( )
{
	Unit* victim = NULL;
	victim = UnitManager::closer( this, &FoodTypes );
	if( victim )
		eat( victim );
}

void DynamicUnit::eat( Unit* victim )
{
	//TODO: пересмотреть надо бы
	if( !victim->isEdible( ) )
		return;
	float fed = Parameters["fed"];
	float level = Parameters["level"];
	float hp = Parameters["hp"];
	float hpmax = Parameters["hpmax"];
	float dmg = Parameters["damage"] / fed * 100;
	if( level == 0 ) level = 1;
	victim->hit( dmg );
	if( fed >= 80 && fed < 100 && hp < hpmax ){
		float hpAdd = dmg / level;
		if( (hp + hpAdd) > hpmax )
			Parameters["hp"] = hpmax;
		else
			Parameters["hp"] += hpAdd;
	}
	if( fed >= 100 ){
		Parameters["fed"] = 100;
		if( hp < hpmax ){
			float hpAdd = dmg;
			if( ( hp + hpAdd ) > hpmax )
				Parameters["hp"] = hpmax;
			else
				Parameters["hp"] += hpAdd;
		}
	}else{
		float n = victim->getUnitParameter("nutritive");
		float fedAdd = dmg * n / level;
		if( ( fed + fedAdd ) > 100 )
			Parameters["fed"] = 100;
		else
			Parameters["fed"] += fedAdd;
	}
}

void DynamicUnit::Die( )
{
	Corpse* corpse;
	corpse = dynamic_cast<Corpse*>( UnitManager::CreateUnit( utCorpse, getUnitX(), getUnitY() ) );
	if( corpse ){
		LuaConfig* cfg = new LuaConfig;
		std::vector<int> bcolor;
		cfg->getValue( "bloodcolor", UnitName, Type, bcolor );
		if( bcolor.size() >= 3 )
			corpse->setBloodColor( bcolor[0], bcolor[1], bcolor[2] );
		else if( bcolor.size() >= 1 )
			corpse->setBloodColor( bcolor[0] );
		corpse->setUnitParameter( "hp", getUnitParameter( "hpmax" ) * getUnitParameter( "fed" ) / 100 );
		corpse->setUnitSize( Image.getSize() );
		delete cfg;
	}
	if( this->Attacked ){
		this->Attacked->increaseUnitParameter( "exp", getUnitParameter( "hpmax" ) / getUnitParameter( "level" ) );
		this->Attacked->increaseUnitParameter( "kills" );
	}
	this->Delete();
}

void DynamicUnit::levelUp( int addlevel )
{
	//FIXME: level not decreased;
	float scale;
	float hp;
	float hpmax;
	float expmax;
	float level;
	for( int i = 1; i<= addlevel; ++i){
		level = ++Parameters["level"];
		scale = ( log( level ) / log( 40.0f ) );
		if( scale < 0.35 )
			scale = 0.35f;
		else if( scale > 1.3 )
			scale = 1.3f;
		Image.setSize( scale );
		hpmax = Parameters["hpmax"];
		if( hpmax == 0 ) hpmax = 1;
		expmax = Parameters["expmax"];
		hp = Parameters["hp"] / hpmax;
		Parameters["exp"] -= expmax;
		Parameters["hpmax"] += 10 * level;
		Parameters["expmax"] = expmax + log( level / 40.0f );
		Parameters["hp"] = Parameters["hpmax"] * hp;
	}
}

void DynamicUnit::update( const int& dt )
{
	AnimatedUnit::update( dt );
	if( getUnitParameter( "hp" ) <= 0 ){
		Die( );
		return;
	}
	if( this->Parameters["exp"] >= this->Parameters["expmax"] )
		levelUp(1);
}

void DynamicUnit::takeAction( )
{
	AnimatedUnit::takeAction();
	if( Parameters["fed"] > 1 )
		Parameters["fed"] -= 0.2f * log( Parameters["level"] );
	if( Attacked ){
		if( Attacked->isDeleted() || Attacked->getUnitParameter( "hp" ) <= 0 || dist(Attacked) >= 1000 ){
			Attacked = NULL;
		}
	}
}

void DynamicUnit::attack( )
{
	Unit* victim = NULL;
	victim = UnitManager::closer( this, "entity", 120.0 );
	if( victim )
		this->attackUnit( victim );
}

void DynamicUnit::attackUnit( Unit* victim )
{
	if( victim == this || !victim )
		return;
	if( Parameters["fed"] > 1 )
		Parameters["fed"] -= 0.1f;
	float dmg =  Parameters["damage"] * Parameters["fed"] / 100;
	if( dmg < 0.4 )
		dmg = 0.4f;
	DynamicUnit* dvictim = dynamic_cast<DynamicUnit*>(victim);
	if( dvictim && dvictim->Attacker() != this )
		dvictim->Attacker( this );
	victim->hit( dmg );
}

void DynamicUnit::hit( float damage )
{
	Image.getSprite()->clr.set( 255, 0, 0 );
	Parameters["hp"] -= damage;
}
