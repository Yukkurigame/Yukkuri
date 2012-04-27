
#include "Unit.h"
#include "unitmanager.h" //FUUUUUUUUUUUUU~
#include <cmath>

#include "LuaConfig.h"


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
