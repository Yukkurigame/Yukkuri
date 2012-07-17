
#include "Unit.h"
#include "unitmanager.h" //FUUUUUUUUUUUUU~

#include "hacks.h"
#include "safestring.h"
#include <cmath>

#include "scripts/LuaConfig.h"
#include "scripts/proto.h"


Unit::Unit()
{
	UnitId = 0;
	X = 0.0;
	Y = 0.0;
	Z = 1;
	DT = 0;
	UnitName = "";
	Type = "";
	flags = 0;
}


Unit::~Unit()
{

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


		// Different
		case acSetUnitSize:
			if( Actions.checkFrameParams( frame, 1, stInt ) )
				setUnitSize( static_cast<float>(param.intData) / 100 );
			break;

		default:
			return false;
	}
	return true;
}

#undef PARAMCOND
#undef PARAMSCOND
#undef CHECKFLAG



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
	X = x;
	Y = y;
	Image.setPosition( x, y, Z );

}

float Unit::dist( Unit* target )
{
	if( !target )
		return 0; //Lol, Nobody here!
	float x = X - target->getUnitX();
	float y = Y - target->getUnitY();
	return sqrt( ( x * x ) + ( y * y ) );
}
