
#include "Unit.h"
#include "unitmanager.h" //FUUUUUUUUUUUUU~

#include "hacks.h"
#include "safestring.h"
#include <cmath>

#include "scripts/LuaConfig.h"
#include "scripts/proto.h"
#include "3rdparty/timer/TimerManager.h"


void call_velocity_func(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt)
{
	( (Unit*)body->data )->velocityFunc();
}


Unit::Unit()
{
	UnitId = 0;
	Z = 1;
	DT = 0;
	UnitName = "";
	Type = "";
	flags = 0;
	actionTimers = NULL;
	physBody = cpBodyNew( 1.0, 1.0 );
	physBody->data = this;
	physShape = NULL;
}


Unit::~Unit()
{
	ActionTimer* timer = NULL;
	while( actionTimers != NULL ){
		timer = actionTimers;
		Timer::DeleteTimerEventById( timer->timerId );
		actionTimers = timer->next;
		delete timer, timer = NULL;
	}
	if( physBody ){
		cpSpaceRemoveBody( Phys::space, physBody );
		cpBodyFree( physBody );
	}
}


bool Unit::Create( int id, std::string proto )
{
	UnitId = id;

	if( !setUnitName( Type ) )
		return false;

	if( !Image.init( UnitName, Type ) )
		return false;

	if( proto == "" ){
		LuaConfig* cfg = new LuaConfig;
		cfg->getValue( "proto", UnitName, Type, proto );
		delete cfg;
	}
	if( proto != "" ){
		ProtoManager* pm = new ProtoManager;
		Actions.setProto( pm->GetProtoByName( proto ) );
		Actions.setAction( "init" );
		delete pm;
	}
	cpSpaceAddBody( Phys::space, physBody );
	//physBody->velocity_func = call_velocity_func;

	return true;
}

void Unit::update( const int& dt )
{
	DT = dt;
	if( isDeleted() )
		return;
	if( Char.state.hp < 0 )
		return die();
	if( dist( UnitManager::GetPlayer( ) ) > 2000 ){
		return setDeleted();
	}

	Actions.updateTimers( dt );

	if( physBody )
		Image.setPosition( physBody->p.x, physBody->p.y );

	if( !Actions.loaded )
		return;

	while( !Actions.nextFrame( ) ){
		const Frame& frame = Actions.action->frames[Actions.frame];

		if( frame.func != LUA_NOREF ){

			extern LuaScript* luaScript;

			this->pushUData( luaScript->getState() );
			int ret_val = luaScript->ExecChunkFromReg( frame.func, 1 );
			if( ret_val == -1 )	{
				Debug::debug( Debug::PROTO,
					"An error occurred while executing a local function. obj id  " +
					citoa(UnitId) + ", proto_name '" + Actions.proto->name + "', action '" +
					Actions.action->name  + "', frame " + citoa(Actions.frame) +
					": " + luaScript->getString( -1 ) + ".\n" );
			}

			for(int i = 0; i > ret_val; ++i ){
				switch( luaScript->getType( 1 ) ){
					case LUA_TNUMBER:
						Actions.params.Push( luaScript->getNumber( 1 ) );
						break;
					case LUA_TSTRING:
						Actions.params.Push( luaScript->getChar( 1 ) );
						break;
				}
			}
		}

		// Make a pause on false result;
		if( !update( frame ) )
			break;

	}

}

#define PARAMCOND(sign)																	\
	if( Actions.checkFrameParams( frame, 2, stInt, stInt ) ){							\
		if( Char.get( frame.params[1].intData ) sign param.intData )					\
			Actions.frame = frame.condition_end;										\
		else																			\
			Debug::debug( Debug::PROTO, "acIfParameterCond bad parameter name.\n" );	\
	}

#define PARAMSCOND(sign)															\
	if( Actions.params.CheckParamTypes( 2, stInt, stInt ) ){						\
		if( Char.get( param.intData ) sign Char.get( frame.params[1].intData ) )	\
			Actions.frame = frame.condition_end;									\
	}else																			\
		Debug::debug( Debug::PROTO, "acIfParametersEqual bad parameter name.\n" );	\

#define CHECKFLAG															\
	if( param.intData < 1 || param.intData >= ufLast )						\
		Debug::debug( Debug::PROTO, "acFlagOperation bad flag name.\n" );	\
	else


bool Unit::update( const Frame& frame )
{
	const StackData& param = frame.params[0];

	switch(frame.command){
		case acNone:
			break;

		// Functions
		case acSuper:
			Actions.saveState( true );
			Actions.setParentAction( param.stringData );
			break;
		case acRestoreState:
			// This is return for acSuper command. It does nothing if no saved state .
			Actions.restoreState();
			break;
		case acSetAction:
			// If param is not null, it will be action call, not replacing
			if( Actions.checkFrameParams( frame, 1, stStringOrNone ) ){
				Actions.setAction( param.stringData );
				return false;
			}
			break;
		case acSetTimer:
			if( Actions.checkFrameParams( frame, 2, stFunction, stInt ) ){
				ActionTimer* timer = actionTimers;
				const Frame* pframe = &frame;
				while( timer != NULL ){
					if( timer->frame == pframe )
						break;
					timer = timer->next;
				}
				if( !timer || !Timer::UpdateEventById( timer->timerId, frame.params[1].intData) ){
					if( timer ){
						ActionTimer* oldtimer = actionTimers;
						while( timer != NULL ){
							// ActionTimer is deleted from manager. Delete it from holder.
							if( oldtimer->next == timer ){
								oldtimer->next = oldtimer->next->next;
								delete timer, timer = NULL;
								break;
							}
							timer = timer->next;
						}
					}
					IActionTimer* t = new IActionTimer( this, param.intData );
					actionTimers = new ActionTimer( pframe, t, actionTimers );
					actionTimers->timerId = Timer::AddInternalTimerEvent( t, frame.params[1].intData );
				}
			}
			break;

		// Action parameters stack
		case acPushInt:
			Actions.params.Push( param.intData );
			break;
		case acPushFloat:
			//TODO: float
			Actions.params.Push( param.intData );
			Debug::debug( Debug::PROTO, "acPushFloat not implemented.\n" );
			break;
		case acPushString:
			Actions.params.Push( param.stringData );
			break;
		case acPush:
			for( int i = 0; i < FRAME_PARAMS_COUNT; ++i ){
				switch( frame.param_types[i] ){
					case stInt:
						Actions.params.Push( frame.params[i].intData );
						break;
					case stString:
						Actions.params.Push( frame.params[i].stringData );
						break;
					default:
						break;
				}
			}
			break;

		// Conditions
		case acCondition: // acCondition always false
			Actions.frame = frame.condition_end;
			break;
		case acEnd: // Do nothing
			break;



		case acIfParamEqual:
			PARAMCOND( != )
			break;
		case acIfParamLess:
			PARAMCOND( > )
			break;
		case acIfParamMore:
			PARAMCOND( < )
			break;


		case acIfParametersEqual:
			PARAMSCOND( != )
			break;
		case acIfParametersLess:
			PARAMSCOND( > )
			break;
		case acIfParametersMore:
			PARAMSCOND( < )
			break;
		case acIfParametersLessBy:
			PARAMSCOND( + param.intData > )
			break;



		// Unit flags
		case acIfFlag:
			CHECKFLAG
				if( !( flags & param.intData ) )
					Actions.frame = frame.condition_end;
			break;
		case acIfNotFlag:
			CHECKFLAG
				if( flags & param.intData )
					Actions.frame = frame.condition_end;
			break;
		case acSetFlag:
			CHECKFLAG
				flags |= param.intData;
			break;
		case acRemoveFlag:
			CHECKFLAG
				flags &= ~param.intData;
			break;



		// Unit Parameters
		case acSetParam:
			if( Actions.checkFrameParams( frame, 2, stInt, stInt ) )
				Char.set( param.intData, frame.params[1].intData );
			else
				Debug::debug( Debug::PROTO, "acSetParam bad parameter name.\n" );
			break;
		case acCopyParam:
			if( !Actions.checkFrameParams( frame, 2, stInt, stInt ) )
				Debug::debug( Debug::PROTO, "acCopyParam bad original parameter name.\n" );
			else if( !param.intData )
				Debug::debug( Debug::PROTO, "acCopyParam bad parameter name.\n" );
			else
				Char.set( param.intData, Char.get( frame.params[1].intData ) );
			break;
		case acLoadParam:
		{
			if( !Actions.checkFrameParams( frame, 2, stInt, stString ) )
				break;
			LuaConfig* cfg = new LuaConfig;
			if( param.intData < uCharIntLast )
				cfg->getValue( frame.params[1].stringData, UnitName, Type,
								Char.getRef( (enum character)param.intData ) );
			else
				cfg->getValue( frame.params[1].stringData, UnitName, Type,
								Char.getRef( (enum character_float)param.intData ) );
			delete cfg;
			break;
		}
		case acLoadParamBunch:
		{
			if( param.intData <= 0 )
				break;
			LuaConfig* cfg = new LuaConfig;
			for( int i = 0; i < param.intData; i++ ){
				if( !Actions.params.CheckParamTypes( 2, stInt, stString ) ){
					Debug::debug( Debug::PROTO, "acLoadPraramBunch wrong " +
							citoa(i+1) + " parameter set.\n" );
					continue;
				}
				int psparam = Actions.params.PopInt();
				if( psparam < uCharIntLast )
					cfg->getValue( Actions.params.PopString(), UnitName, Type,
							Char.getRef( (enum character)psparam ) );
				else
					cfg->getValue( Actions.params.PopString(), UnitName, Type,
							Char.getRef( (enum character_float)psparam ) );
			}
			delete cfg;
			break;
		}


		// Physics
		case acSetUnitPhysics:
			if( Actions.checkFrameParams( frame, 3, stInt, stInt, stIntOrNone ) ){
				int firstparam = frame.params[1].intData;
				switch(param.intData){
					case pptMat:
						break;
					case pptType:
						if( firstparam >= 0 && firstparam < potLast )
							phys.type = (enum PhysObectType)firstparam;
						else
							Debug::debug( Debug::UNIT, "acSetUnitPhysics: bad physics object type." );
						break;
					case pptRadius:
						phys.radius = firstparam;
						break;
					case pptSides:
						phys.sides.x = firstparam;
						phys.sides.y = frame.params[2].intData;
						break;
				}
				phys.calc_mass();
				updatePhysics( );
			}
			break;
		case acSetPhysicsLayer:
			if( Actions.checkFrameParams( frame, 1, stInt ) && physShape != NULL )
				cpShapeSetGroup( physShape, param.intData );
			break;


		// Misc
		case acSetUnitSize:
			if( Actions.checkFrameParams( frame, 1, stInt ) )
				setUnitSize( static_cast<float>(param.intData) / 100 );
			break;
		case acSetColor:
			if( Actions.checkFrameParams( frame, 4, stInt, stInt, stInt, stInt ) )
				Image.getSprite()->clr.set( frame.params[0].intData, frame.params[1].intData,
						frame.params[2].intData, frame.params[3].intData );
			break;

		default:
			return false;
	}
	return true;
}

#undef PARAMCOND
#undef PARAMSCOND
#undef CHECKFLAG


void Unit::updatePhysics( )
{
	cpBodySetMass( physBody, phys.mass );
	cpFloat moment;
	switch(phys.type){
		case potCircle:
			moment = cpMomentForCircle( phys.mass, 0, phys.radius, cpvzero );
			break;
		default:
			moment = 1.0;
			break;
	}
	cpBodySetMoment( physBody, moment );
}


void Unit::setUnitType( enum unitType type )
{
	UnitType = type;
	switch( type ){
		case utPlant:
			Type = "plant";
			break;
		case utCorpse:
			Type = "corpse";
			break;
		default:
			Type = "entity";
			break;
	}
}

bool Unit::setUnitName( std::string type )
{
	//FIXME: it's bad.
	LuaConfig* cfg = new LuaConfig;
	UnitName = cfg->getRandom("meeting", type);
	delete cfg;
	if( UnitName == "" )
		return false;
	return true;
}

void Unit::setUnitPos( float x, float y )
{
	cpVect v = {x, y};
	cpBodySetPos( physBody, v );
	Image.setPosition( x, y, Z );

}

float Unit::dist( Unit* target )
{
	if( !target )
		return 0; //Lol, Nobody here!
	float x = physBody->p.x - target->getUnitX();
	float y = physBody->p.y - target->getUnitY();
	return sqrt( ( x * x ) + ( y * y ) );
}

void Unit::setUnitSize( float size )
{
	Image.setSize(size);
	phys.scale( size );
	updatePhysics( );

}
