
#include "UnitDynamic.h"
#include "UnitCorpse.h"

#include "unitmanager.h"

#include "scripts/LuaConfig.h"
#include "graphics/AnimationDefines.h"
#include "graphics/Render.h"

#include "map/Map.h"
#include "config.h"

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>

extern MainConfig conf;



void call_updateAnimOnMovement(cpBody* body, cpFloat dt)
{
	((UnitDynamic* )body->data)->updateAnimOnMovement( body, dt );
}


UnitDynamic::UnitDynamic()
{
	TotalDistance = 0;
	Attacked = NULL;
	currentTile = -1;
	force.x = 0;
	force.y = 0;
	scopeShape = NULL;
	vis = NULL;
}

UnitDynamic::~UnitDynamic()
{
	if( scopeShape ){
		cpSpaceRemoveShape( Phys::space, scopeShape );
		cpShapeFree( scopeShape );
	}
	if( vis )
		RenderManager::FreeGLSprite( vis );
}

bool UnitDynamic::Create( int id, std::string proto )
{
	if( !Unit::Create( id, proto ) )
		return false;

	physBody->position_func = call_updateAnimOnMovement;

	cpVect scopepos[4] = { {-400.0 * cos(conf._tileAngle), 0}, {0, 400.0 * sin(conf._tileAngle) },
							{400 * cos(conf._tileAngle), 0}, {0, -400.0 * sin(conf._tileAngle) }  };
	//cpVect scopepos[4];
	//cpConvexHull( 4, scopepoints, scopepos, NULL, 0.0 );

	cpShape* scope = cpPolyShapeNew( physBody, 4, scopepos, cpvzero );
	scopeShape = cpSpaceAddShape( Phys::space, scope );
	cpShapeSetSensor( scopeShape, cpTrue );
	scopeShape->collision_type = utLast;

	return true;
}



/** Move the unit according to the eight cardinal directions.
	@return Boolean value represents a Collision (true)
	@remark
**/
void UnitDynamic::moveUnit( signed int x, signed int y )
{
	/*if( x != 0 || y != 0 ){
		float zone = 1.0;
		float l = sqrt( static_cast<float>(x * x  +  y * y) );
		float speed = fabs( Char.chars.speed * Char.params.fed ) * zone * ( DT / 100000.0f ) / l;
		float dx = speed * x; // / l;
		float dy = speed * y; // / l;
		*/
		/* //MapTile* currentTile = map.GetTile( X , Y );
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
		/*dx *= zone;
		dy *= zone;
		float distance = sqrt( dx * dx + dy * dy );
		TotalDistance += distance;
		if( dx || dy ) { //FUUU
			if( dx < 0 && dy > 0 )
				Image.setAnimation(AnimDef::leftup);
			else if( dx < 0 && dy < 0 )
				Image.setAnimation(AnimDef::leftdown);
			else if( dx > 0 && dy > 0 )
				Image.setAnimation(AnimDef::rightup);
			else if( dx > 0 && dy < 0 )
				Image.setAnimation(AnimDef::rightdown);
			else if( dy > 0 )
				Image.setAnimation(AnimDef::up);
			else if( dy < 0 )
				Image.setAnimation(AnimDef::down);
			else if( dx < 0 )
				Image.setAnimation(AnimDef::left);
			else if( dx > 0 )
				Image.setAnimation(AnimDef::right);
			Image.setFrame( Image.getCount() ? ( static_cast<int>(TotalDistance) / m_animdistance) % Image.getCount() : 0 );
		}
		unitX += dx;
		unitY += dy;
		Image.setPosition(unitX, unitY);
		Char.tire( distance / 200 );

	}*/
}


void UnitDynamic::eat( )
{
	Unit* victim = NULL;
	//victim = UnitManager::closer( this, &FoodTypes );
	if( victim )
		eat( victim );
}


void UnitDynamic::eat( Unit* victim )
{
	//TODO: пересмотреть надо бы
	if( !victim->isEdible( ) )
		return;

	float dmg = Char.getDamage();
	victim->hit( dmg );
	if( Char.state.fed >= 80 && Char.state.fed < 100 && Char.state.hp < Char.params.hp ){
		float hpAdd = dmg / Char.level;
		if( (Char.state.hp + hpAdd) > Char.params.hp )
			Char.state.hp = Char.params.hp;
		else
			Char.state.hp += hpAdd;
	}
	if( Char.state.fed >= 100 ){
		Char.state.fed = 100;
		if( Char.state.hp < Char.params.hp ){
			if( ( Char.state.hp + dmg ) > Char.params.hp )
				Char.state.hp = Char.params.hp;
			else
				Char.state.hp += dmg;
		}
	}else{
		float n = victim->getUnitParameter( uChrNutritive );
		float fedAdd = dmg * n / Char.level;
		if( ( Char.state.fed + fedAdd ) > 100 )
			Char.state.fed = 100;
		else
			Char.state.fed += fedAdd;
	}
}

void UnitDynamic::die( )
{
	Corpse* corpse;
	corpse = dynamic_cast<Corpse*>( UnitManager::CreateUnit( utCorpse, getUnitX(), getUnitY() ) );
	if( corpse ){
		LuaConfig* cfg = new LuaConfig;
		std::vector<int> bcolor;
		cfg->getValue( "bloodcolor", UnitName, TypeName, bcolor );
		if( bcolor.size() >= 3 )
			corpse->setBloodColor( (unsigned)bcolor[0], (unsigned)bcolor[1], (unsigned)bcolor[2] );
		else if( bcolor.size() >= 1 )
			corpse->setBloodColor( (unsigned)bcolor[0] );
		corpse->setUnitParameter( uParamHP, getUnitParameter( uParamHP ) * getUnitParameter( uParamFed ) / 100 );
		corpse->setUnitSize( Image.getSize() );
		delete cfg;
	}
	if( this->Attacked ){
		this->Attacked->setUnitParameter( uStateExp,
				this->Attacked->getUnitParameter( uStateExp ) +
				Char.get( uParamHP ) / Char.get( uBaseLevel ) );
		this->Attacked->setUnitParameter( uBaseKills,
				(float)this->Attacked->getUnitParameter( uBaseKills ) + 1.0f );
	}
	this->setDeleted();
}


void UnitDynamic::update( const int& dt )
{
	Unit::update( dt );
	if( vis != NULL ){
		cpVect lb = cpPolyShapeGetVert( scopeShape, 0 );
		cpVect lt = cpPolyShapeGetVert( scopeShape, 1 );
		cpVect rt = cpPolyShapeGetVert( scopeShape, 2 );
		cpVect rb = cpPolyShapeGetVert( scopeShape, 3 );
		vis->setPosition( s3f(physBody->p.x + lb.x, physBody->p.y + lb.y, 1.0),
				s3f(physBody->p.x + lt.x, physBody->p.y + lt.y, 1.0),
				s3f(physBody->p.x + rt.x, physBody->p.y + rt.y, 1.0),
				s3f(physBody->p.x + rb.x, physBody->p.y + rb.y, 1.0));
	}
}


bool UnitDynamic::update( const Frame& frame )
{
	switch(frame.command){
		case acDAddFood:
			if( Actions.checkFrameParams( frame, 1, stInt ) ){
				int type = frame.params[0].intData;
				if( type > 0 && type < utLast )
					FoodTypes.push( (enum unitType)type );
			}
			break;
		case acDUnitGrow:
		{
			Char.grow();
			float scale = ( log( (float)Char.get( uBaseLevel ) ) / log( 40.0f ) );
			if( scale < 0.35 )
				scale = 0.35f;
			else if( scale > 1.3 )
				scale = 1.3f;
			setUnitSize( scale );
			break;
		}
		default:
			return Unit::update( frame );
	}
	return true;
}

void UnitDynamic::takeAction( )
{
	Char.tire();
	if( Attacked ){
		if( Attacked->isDeleted() || Attacked->getUnitParameter( uStateHP ) <= 0 || dist(Attacked) >= 1000 ){
			Attacked = NULL;
		}
	}
}

void UnitDynamic::grow( )
{
	Unit::grow();
	float scale = ( log( (float)Char.get( uBaseLevel ) ) / log( 40.0f ) );
	if( scale < 0.35 )
		scale = 0.35f;
	else if( scale > 1.3 )
		scale = 1.3f;
	setUnitSize( scale );
}


void UnitDynamic::updateAnimOnMovement( cpBody* body, cpFloat dt )
{
	cpVect oldpos = body->p;
	cpBodyUpdatePosition( body, dt );
	cpFloat distance = cpvdist( oldpos, body->p );
	TotalDistance += distance;
	Char.tire( distance / 200 );
	if( !cpveql( body->v, cpvzero ) ) { //FUUU
		if( body->v.x < 0 && body->v.y > 0 )
			Image.setAnimation(AnimDef::leftup);
		else if( body->v.x < 0 && body->v.y < 0 )
			Image.setAnimation(AnimDef::leftdown);
		else if( body->v.x > 0 && body->v.y > 0 )
			Image.setAnimation(AnimDef::rightup);
		else if( body->v.x > 0 && body->v.y < 0 )
			Image.setAnimation(AnimDef::rightdown);
		else if( body->v.y > 0 )
			Image.setAnimation(AnimDef::up);
		else if( body->v.y < 0 )
			Image.setAnimation(AnimDef::down);
		else if( body->v.x < 0 )
			Image.setAnimation(AnimDef::left);
		else if( body->v.x > 0 )
			Image.setAnimation(AnimDef::right);
		Image.setFrame( Image.getCount() ? ( static_cast<int>(TotalDistance) / m_animdistance) % Image.getCount() : 0 );
	}
}


void UnitDynamic::setScope()
{
	if( vis == NULL ){
		vis = RenderManager::CreateGLSprite( 0, 0, 1, 600, 300 );
		vis->clr.set( 255, 255, 0, 127 );
	}
	vis->setVisible();
}


void UnitDynamic::clearScope()
{
	if( vis != NULL ){
		RenderManager::FreeGLSprite( vis );
		vis = NULL;
	}
}


void UnitDynamic::attack( )
{
	Unit* victim = NULL;
	victim = UnitManager::closer( this, utEntity, 120.0 );
	if( victim )
		this->attackUnit( victim );
}

void UnitDynamic::attackUnit( Unit* victim )
{
	if( victim == this || !victim )
		return;
	Char.tire( 0.1f );
	UnitDynamic* dvictim = dynamic_cast<UnitDynamic*>(victim);
	if( dvictim && dvictim->Attacker() != this )
		dvictim->Attacker( this );
	victim->hit( Char.getDamage() );
}

void UnitDynamic::hit( float damage )
{
	Actions.saveState( true );
	Actions.setAction( "hit" );
	Char.recieveDamage( damage );
}
