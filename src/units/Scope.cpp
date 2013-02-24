/*
 * Scope.cpp
 *
 *  Created on: 22.12.2012
 */

#include "units/Scope.h"
#include "graphics/Render.h"
#include "units/Unit.h"

#include "config.h"

extern MainConfig conf;


#define POSITION_Z 0.99


Scope::~Scope( )
{
	if( shape ){
		cpSpaceRemoveShape( Phys::space, shape );
		cpShapeFree( shape );
	}
	if( sprite )
		RenderManager::FreeGLSprite( sprite );
}


bool Scope::attach( cpBody* target )
{
	if( target == NULL )
		return false;

	cpVect scopepos[4] = { {-400.0 * cos(conf._tileAngle), 0}, {0, 400.0 * sin(conf._tileAngle) },
							{400 * cos(conf._tileAngle), 0}, {0, -400.0 * sin(conf._tileAngle) }  };
	//cpVect scopepos[4];
	//cpConvexHull( 4, scopepoints, scopepos, NULL, 0.0 );

	cpShape* scope = cpPolyShapeNew( target, 4, scopepos, cpvzero );
	shape = cpSpaceAddShape( Phys::space, scope );
	cpShapeSetSensor( shape, cpTrue );
	shape->collision_type = utAll;

	return true;
}


void Scope::update( )
{
	if( sprite && sprite->isVisible() ){
		cpBody* b = shape->body;
		sprite->brush.set_position( b->p.x, b->p.y, POSITION_Z );
	}
}


void Scope::set( )
{
	if( !shape )
		return;

	if( sprite == NULL ){
		int verts = cpPolyShapeGetNumVerts( shape );

		sprite = RenderManager::CreateGLSprite( 0.0, 0.0, POSITION_Z, 600, 300, 0 );
		sprite->brush.resize_verticles( verts );
		sprite->brush.set_color( color );

		s3f* points = sprite->brush.vertex_points;
		for( int i=0; i < verts; ++i ){
			cpVect sv = cpPolyShapeGetVert( shape, i );
			points[i].x = sv.x;
			points[i].y = sv.y;
		}
		sprite->brush.update_points();
	}
	sprite->setVisible();
}


void Scope::clear( )
{
	if( sprite != NULL ){
		if( sprite->isVisible() ){
			sprite->clearVisible();
		}else{
			// Free sprite on second call
			RenderManager::FreeGLSprite( sprite );
			sprite = NULL;
		}
	}
}



Unit* Scope::closest( int type, float limit )
{
	if( !shape )
		return NULL;

	Unit* ret = NULL;
	float distance = 9000;
	cpVect p = shape->body->p;
	Unit* self = reinterpret_cast<Unit*>(shape->body->data);
	listElement< Unit* >* tmp = Collisions.head;
	while( tmp != NULL ){
		Unit* target = tmp->data;
		if( target && target != self && ( target->getUnitType() & type ) ){
			const s2f& tp = target->getUnitPos();
			float dis = cpvlength( cpvsub( p, cpv( tp.x, tp.y ) ));
			if( dis < limit && dis < distance ){
				distance = dis;
				ret = tmp->data;
			}
		}
		tmp = tmp->next;
	}

	return ret;
}

