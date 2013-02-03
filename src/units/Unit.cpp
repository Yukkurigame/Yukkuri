
#include "Unit.h"
#include "unitmanager.h" //FUUUUUUUUUUUUU~

#include "scripts/LuaConfig.h"
#include "scripts/proto.h"
#include "3rdparty/timer/TimerManager.h"

#include "hacks.h"
#include "safestring.h"

#include "chipmunk/chipmunk_unsafe.h"
#include <cmath>


Unit::Unit()
{
	UnitType = utEntity;
	UnitId = 0;
	Z = 1;
	DT = 0;
	UnitName = "";
	TypeName = "";
	flags = 0;
	actionTimers = NULL;
	physShape = NULL;
	physBody = NULL;
	events.function = events.self = LUA_NOREF;
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
	if( physShape ){
		cpSpaceRemoveShape( Phys::space, physShape );
		cpShapeFree( physShape );
	}
	if( physBody ){
		if( !cpBodyIsStatic(physBody) )
			cpSpaceRemoveBody( Phys::space, physBody );
		cpBodyFree( physBody );
	}
}


bool Unit::Create( int id, std::string proto )
{
	UnitId = id;

	if( !setUnitName( TypeName ) )
		return false;

	if( !Image.init( UnitName, TypeName ) )
		return false;

	if( proto == "" ){
		LuaConfig* cfg = new LuaConfig;
		cfg->getValue( "proto", UnitName.c_str(), TypeName.c_str(), proto );
		delete cfg;
	}
	if( proto == "" ){
		Debug::debug( Debug::UNIT, "Unit with blank prototype, creation failed. UID: "
						+ citoa(UnitId) + ".\n" );
		return false;
	}

	{
		ProtoManager* pm = new ProtoManager;
		Actions.setProto( pm->GetProtoByName( proto ) );
		Actions.setAction( Action::getId("init") );
		delete pm;
		if( !Actions.loaded ){
				Debug::debug( Debug::UNIT, "Unit with invalid prototype '" + proto +
					"'. Creation failed. UID: " + citoa(UnitId) + ".\n" );
				return false;
		}
	}

	// Create physics

	if( Actions.proto->statical ){
		physBody = cpBodyNewStatic();
	}else{
		physBody = cpBodyNew( 1.0, 1.0 );
		physBody = cpSpaceAddBody( Phys::space, physBody );
	}
	physBody->data = this;



	if( Actions.proto->physicsType >= 0 && Actions.proto->physicsType < potLast )
		phys.type = (enum PhysObectType)Actions.proto->physicsType;
	else{
		Debug::debug( Debug::UNIT, "Bad physics object type: "
				+ citoa( Actions.proto->physicsType ) + ".\n" );
		phys.type = potNone;
	}

	cpShape* shape = NULL;
	switch( phys.type ){
		case potCircle:
			shape = cpCircleShapeNew( physBody, phys.radius, cpvzero );
			break;
		case potQuad:
			shape = cpBoxShapeNew( physBody, phys.sides.x, phys.sides.y );
			break;
		default:
			break;
	}
	if( shape != NULL ){
		if( Actions.proto->statical )
			physShape = cpSpaceAddStaticShape( Phys::space, shape );
		else
			physShape = cpSpaceAddShape( Phys::space, shape );
		physShape->collision_type = UnitType;
	}

	//physBody->velocity_func = call_velocity_func;

	return true;
}

void Unit::update( const int& dt )
{
	DT = dt;
	if( isDeleted() )
		return;

	// FIXME: move to prototypes
	if( Char.state.hp < 0 ){
		extern LuaScript* luaScript;
		luaScript->push( std::string("die") );
		emitEvent( luaScript->getState() );
		if(isDeleted())
			return;
	}

	if( dist( UnitManager::GetPlayer( ) ) > 2000 ){
		return setDeleted();
	}

	Actions.updateTimers( dt );

	if( physBody )
		Image.setPosition( (float)physBody->p.x, (float)physBody->p.y );

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
					Action::getName(Actions.action->id)  + "', frame " + citoa(Actions.frame) +
					": " + luaScript->getString( -1 ) + ".\n" );
			}

			for(int i = 0; i > ret_val; ++i ){
				switch( luaScript->getType( 1 ) ){
					case LUA_TNUMBER:
						Actions.params.Push( (int)luaScript->getNumber( 1 ) );
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
			Actions.setParentAction( param.intData );
			break;
		case acRestoreState:
			// This is return for acSuper command. It does nothing if no saved state .
			Actions.restoreState();
			break;
		case acSetAction:
			// If param is not null, it will be action call, not replacing
			if( Actions.checkFrameParams( frame, 1, stIntOrNone ) ){
				Actions.setAction( param.intData );
				return false;
			}
			break;
		case acSetTimer:
			if( Actions.checkFrameParams( frame, 3, stFunction, stInt, stIntOrNone ) ){
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
					int period = frame.params[1].intData;
					int calls = 1;
					if( Actions.checkFrameParams( frame, 2, stFunction, stInt ) )
						calls = frame.params[2].intData;
					IActionTimer* t = new IActionTimer( this, param.intData );
					actionTimers = new ActionTimer( pframe, t, actionTimers );
					actionTimers->timerId = Timer::AddInternalTimerEvent(
							t, period, period, calls, (calls == 1) ? false : true, false );
				}
			}
			break;
		case acBindEvent:
			if( Actions.checkFrameParams( frame, 2, stFunction, stTableOrNone ) ){
				this->events.function = param.intData;
				this->events.self = frame.params[1].intData;
			}
			break;
		case acEmitEvent:
			extern LuaScript* luaScript;
			if( Actions.checkFrameParams( frame, 1, stString ) && this->events.function != LUA_NOREF ){
				luaScript->push( param.stringData );
				int ret_val = emitEvent( luaScript->getState() );
				for(int i = 0; i < ret_val; ++i ){
					switch( luaScript->getType( 1 ) ){
						case LUA_TNUMBER:
							Actions.params.Push( (int)luaScript->getNumber( 1 ) );
							break;
						case LUA_TSTRING:
							Actions.params.Push( luaScript->getChar( 1 ) );
							break;
					}
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
				Char.set( param.intData, (float)frame.params[1].intData );
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
				cfg->getValue( frame.params[1].stringData, UnitName.c_str(), TypeName.c_str(),
								Char.getRef( (enum character)param.intData ) );
			else
				cfg->getValue( frame.params[1].stringData, UnitName.c_str(), TypeName.c_str(),
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
					cfg->getValue( Actions.params.PopString().c_str(), UnitName.c_str(), TypeName.c_str(),
							Char.getRef( (enum character)psparam ) );
				else
					cfg->getValue( Actions.params.PopString().c_str(), UnitName.c_str(), TypeName.c_str(),
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
			if( Actions.checkFrameParams( frame, 4, stInt, stInt, stInt, stInt ) ){
				s4ub color( frame.params[0].intData, frame.params[1].intData,
						frame.params[2].intData, frame.params[3].intData);
				Image.getSprite()->brush.set_color( color );
			}
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
			if( physShape )
				cpCircleShapeSetRadius( physShape, phys.radius );
			moment = cpMomentForCircle( phys.mass, 0, phys.radius, cpvzero );
			break;
		default:
			moment = 1.0;
			break;
	}
	cpBodySetMoment( physBody, moment );
	if( cpBodyIsStatic(physBody) && phys.type != potNone )
		cpSpaceReindexShape( Phys::space, physShape );
}


void Unit::setUnitType( enum unitType type )
{
	UnitType = type;
	switch( type ){
		case utPlant:
			TypeName = "plant";
			break;
		case utCorpse:
			TypeName = "corpse";
			break;
		default:
			TypeName = "entity";
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
	if( cpBodyIsStatic(physBody) && phys.type != potNone )
		cpSpaceReindexShape( Phys::space, physShape );
}


float Unit::dist( Unit* target )
{
	if( !target )
		return 0; //Lol, Nobody here!
	float x = (float)physBody->p.x - target->getUnitX();
	float y = (float)physBody->p.y - target->getUnitY();
	return sqrt( ( x * x ) + ( y * y ) );
}

void Unit::setUnitSize( float size )
{
	Image.setSize(size);
	phys.scale( size );
	updatePhysics( );

}
