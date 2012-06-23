
#include "Unit.h"
#include "unitmanager.h" //FUUUUUUUUUUUUU~

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
	UnitName = "";
	Type = "";
	flags = 0;
}


Unit::~Unit()
{

}


bool Unit::Create( int id )
{
	UnitId = id;

	if( !setUnitName( Type ) )
		return false;

	if( !Image.init( UnitName, Type ) )
		return false;

	//FIXME: ololo
	std::string protoname;
	LuaConfig* cfg = new LuaConfig;
	cfg->getValue( "proto", UnitName, Type, protoname );
	if( protoname != "" ){
		ProtoManager* pm = new ProtoManager;
		Actions.setProto( pm->GetProtoByName( protoname ) );
		Actions.setAction( "init" );
		delete pm;
	}

	delete cfg;

	return true;
}

void Unit::update( const int& )
{
	if( isDeleted() )
		return;
	if( Char.state.hp < 0 )
		return die();
	if( dist( UnitManager::GetPlayer( ) ) > 2000 ){
		return setDeleted();
	}


	if( !Actions.loaded )
		return;

	int param = 0;
	const char* txt_param = NULL;

	ParametersStack* ps = &Actions.params;

	while( !Actions.nextFrame() ){
		const Frame& frame = Actions.action->frames[Actions.frame];

		if( frame.is_param_function ){

			extern LuaScript* luaScript;

			int ret_val = luaScript->ExecChunkFromReg( frame.param );
			if( ret_val == -1 )	{
				Debug::debug( Debug::PROTO,
					"An error occurred while executing a local function. obj id  " +
					citoa(UnitId) + ", proto_name '" + Actions.proto->name + "', action '" +
					Actions.action->name  + "', frame " + citoa(Actions.frame) +
					": " + luaScript->getString( -1 ) + ".\n" );
			}

			const int top = luaScript->top();
			const int p1 = top - ret_val + 1;
			const int p2 = top - ret_val + 2;

			param     = (ret_val > 0 ) ? luaScript->getNumber( p1 ) : 0;
			txt_param = ((ret_val > 1 ) ? luaScript->getString( p2 ) : "").c_str();

		}else{
			param = frame.param;
			txt_param = frame.txt_param;
		}

		switch(frame.command){
			case acNone:
				break;

			// Functions
			case acSuper:
				Actions.saveState();
				Actions.setParentAction( txt_param );
				break;

			// Action parameters stack
			case acPushInt:
				ps->Push( param );
				break;
			case acPushFloat:
				//TODO: float
				ps->Push( param );
				Debug::debug( Debug::PROTO, "acPushFloat not implemented.\n" );
				break;
			case acPushString:
				ps->Push( txt_param );
				break;

			// Unit Parameters
			case acSetParam:
				if( ps->CheckParamTypes( 1, stInt ) ){
					int psparam = ps->PopInt();
					if( psparam < uCharIntLast )
						Char.set( (enum character)psparam, param );
					else
						Char.set( (enum character_float)psparam, param );
				}else
					Debug::debug( Debug::PROTO, "acSetParam bad parameter name.\n" );
				break;
			case acCopyParam:
				if( !ps->CheckParamTypes( 1, stInt ) )
					Debug::debug( Debug::PROTO, "acCopyParam bad original parameter name.\n" );
				else if( !param )
					Debug::debug( Debug::PROTO, "acCopyParam bad parameter name.\n" );
				else{
					int psparam = ps->PopInt();
					float sparam;
					if( psparam < uCharIntLast )
						sparam = Char.get( (enum character)psparam );
					else
						sparam = Char.get( (enum character_float)psparam );
					if( param < uCharIntLast )
						Char.set( (enum character)param, sparam );
					else
						Char.set( (enum character_float)param, sparam );
				}
				break;
			case acLoadParam:
			{
				if( !txt_param ){
					Debug::debug( Debug::PROTO, "acLoadPraram bad parameter name.\n" );
					break;
				}else if ( !param ){
					Debug::debug( Debug::PROTO, "acLoadPraram bad target parameter name.\n" );
					break;
				}
				LuaConfig* cfg = new LuaConfig;
				if( param < uCharIntLast )
					cfg->getValue( txt_param, UnitName, Type,
									Char.getRef( (enum character)param ) );
				else
					cfg->getValue( txt_param, UnitName, Type,
									Char.getRef( (enum character_float)param ) );
				delete cfg;
				break;
			}
			case acLoadParamBunch:
			{
				if( param <= 0 )
					break;
				LuaConfig* cfg = new LuaConfig;
				for( int i = 0; i < param; i++ ){
					if( !ps->CheckParamTypes( 2, stInt, stString ) ){
						Debug::debug( Debug::PROTO, "acLoadPraramBunch wrong " +
								citoa(i+1) + " parameter set.\n" );
						continue;
					}
					int psparam = ps->PopInt();
					if( psparam < uCharIntLast )
						cfg->getValue( ps->PopString(), UnitName, Type,
								Char.getRef( (enum character)psparam ) );
					else
						cfg->getValue( ps->PopString(), UnitName, Type,
								Char.getRef( (enum character_float)psparam ) );
				}
				delete cfg;
				break;
			}


		}
	}

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
	X = x;
	Y = y;
	Image.setPosition( x, y, Z );

}

void Unit::setUnitSize( float size )
{
	Image.setSize(size);
}

float Unit::dist( Unit* target )
{
	if( !target )
		return 0; //Lol, Nobody here!
	float x = X - target->getUnitX();
	float y = Y - target->getUnitY();
	return sqrt( ( x * x ) + ( y * y ) );
}
