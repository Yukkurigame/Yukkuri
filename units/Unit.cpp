
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
	Deleted = false;
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
		Prototype.setProto( pm->GetProtoByName( protoname ) );
		delete pm;
	}

	delete cfg;

	return true;
}

void Unit::Delete( )
{
	Deleted = true;
}

void Unit::update( const int& )
{
	if( dist( UnitManager::units.GetPlayer( ) ) > 2000 ){
		this->Delete();
	}


	if( !Prototype.loaded )
		return;

	int param;
	const char* txt_param;

	while( !Prototype.nextFrame() ){
		const Frame& frame = Prototype.action->frames[Prototype.frame];

		if( frame.is_param_function){

			LuaScript* ls = new LuaScript;

			int ret_val = ls->ExecChunkFromReg( frame.param );
			if( ret_val == -1 )	{
				Debug::debug( Debug::CONFIG,
					"An error occurred while executing a local function. obj id  " +
					citoa(UnitId) + ", proto_name '" + Prototype.proto.name + "', action '" +
					Prototype.action->name  + "', frame " + citoa(Prototype.frame) +
					": " + ls->getString( -1 ) + ".\n" );
			}

			const int top = ls->top();
			const int p1 = top - ret_val + 1;
			const int p2 = top - ret_val + 2;

			param     = (ret_val > 0 ) ? ls->getNumber( p1 ) : 0;
			txt_param = ((ret_val > 1 ) ? ls->getString( p2 ) : "").c_str();

		}else{
			param = frame.param;
			txt_param = frame.txt_param;
		}

		switch(frame.command){
			case acNone:
				break;
		}
	}

}

void Unit::setUnitType( enum unitType type )
{
	UnitType = type;
	switch( type ){
		case PLANT:
			Type = "plant";
			break;
		case CORPSE:
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

void Unit::setUnitParameter( std::string name, float value )
{
	Parameters[name] = value;
}

void Unit::increaseUnitParameter( std::string name, float value )
{
	Parameters[name] += value;
}


float* Unit::getUnitpParameter( std::string name )
{
	if( Parameters.count(name) > 0 )
		return &Parameters[name];
	return NULL;
}

float Unit::dist( Unit* target )
{
	if( !target )
		return 0; //Lol, Nobody here!
	float x = X - target->getUnitX();
	float y = Y - target->getUnitY();
	return sqrt( ( x * x ) + ( y * y ) );
}