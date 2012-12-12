
#include "UnitDynamic.h"
#include "UnitCorpse.h"

#include "unitmanager.h"

#include "scripts/LuaConfig.h"
#include "graphics/sprite/AnimationDefines.h"
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

/** Set unit's desire location.
**/
bool UnitDynamic::moveUnit( signed int x, signed int y )
{
	this->target.x = x;
	this->target.y = x;
	return calculateForce();
}

bool UnitDynamic::calculateForce( )
{
	force = cpvzero;
	int nx = target.x - static_cast<int>(getUnitX());
	int ny = target.y - static_cast<int>(getUnitY());
	if( abs(nx) > phys.radius * 3 )
		force.x = nx / abs(nx);
	if( abs(ny) > phys.radius * 3 )
		force.y = ny / abs(ny);
	if( force == cpvzero && physBody->v == cpvzero ){
		//cpBodyResetForces( physBody );
		//cpBodySetVel( physBody, cpvzero );
		clearMoving();
		return false;
	}
	setMoving();
	return true;
}

void UnitDynamic::applyForce( const int& dt )
{
	// Total force delta
	cpVect dforce;
	// Max force
	cpVect mforce = cpvzero;
	// Use half of speed when both forces applied
	cpFloat add = (0.5 * abs(force.x) * abs(force.y));
	// Max velocity
	cpVect mvel = cpvmult(
		cpv(1.0 - add, (1.0 - add) / 2),
		//FIXME: Magic value must be function of time delta
		(Char.chars.speed / 3000.0) * cpfclamp(Char.state.fed / 100.0, 0.1, 1.0)
	);
	// Velocity delta
	cpVect dvel = cpvsub( mvel, cpvmult( physBody->v, Phys::space->damping ) );
	mforce.x = force.x * dvel.x * phys.mass;
	mforce.y = force.y * dvel.y * phys.mass;
	dforce = cpvsub( mforce, physBody->f );
	//physBody->v_limit = p;
	cpBodyApplyForce( physBody, dforce, cpvzero );
}


void UnitDynamic::eat( Unit* victim )
{
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
	if( isMoving() && this->calculateForce( ) )
		applyForce( dt );
	if( vis != NULL ){
		cpVect lb = cpPolyShapeGetVert( scopeShape, 0 );
		cpVect lt = cpPolyShapeGetVert( scopeShape, 1 );
		cpVect rt = cpPolyShapeGetVert( scopeShape, 2 );
		cpVect rb = cpPolyShapeGetVert( scopeShape, 3 );
		vis->brush.set_quad( s3f(lb.x, lb.y, 1.0),
				s3f(lt.x, lt.y, 1.0),
				s3f(rt.x, rt.y, 1.0),
				s3f(rb.x, rb.y, 1.0));
		vis->brush.set_position( physBody->p.x, physBody->p.y, 1.0 );
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


Unit* UnitDynamic::closest( enum unitType type, float limit)
{
	Unit* ret = NULL;
	float distance = 9000;
	limit *= getUnitSize();
	listElement< Unit* >* tmp = Collisions.head;
	while( tmp != NULL ){
		if( tmp->data && tmp->data->getUnitType() == type ){
			float dis = this->dist( tmp->data );
			if( dis < limit && dis < distance ){
				distance = dis;
				ret = tmp->data;
			}
		}
		tmp = tmp->next;
	}

	return ret;
}

Unit* UnitDynamic::closest( list< enum unitType >* types, float limit )
{
	Unit* ret = NULL;
	float distance = 9000;
	limit += phys.radius;
	listElement< Unit* >* tmp = Collisions.head;
	listElement< enum unitType >* le;
	while( tmp != NULL ){
		if( tmp->data ){
			le = types->head;
			while( le != NULL ){
				if( tmp->data->getUnitType() == le->data ){
					float dis = this->dist( tmp->data );
					if( dis < limit && dis < distance ){
						distance = dis;
						ret = tmp->data;
					}
				}
				le = le->next;
			}
		}
		tmp = tmp->next;
	}

	return ret;
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
	}else{
		Image.setFrame( 0 );
	}
}


void UnitDynamic::setScope()
{
	if( vis == NULL ){
		vis = RenderManager::CreateGLSprite( 0, 0, 1, 600, 300 );
		//vis->clr.set( 255, 255, 0, 127 );
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
