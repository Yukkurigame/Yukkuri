#include "unit.h"

Unit::Unit()
{
    m_iAn = 0;
    m_fX = 0.0;
    m_fY = 0.0;
    player = false;
}

Unit::~Unit()
{

}

bool Unit::Create( )
{

	setUnitName();

	//FIXME: where is my debug??
	defs = new EntityDefs;
	//FIXME: It,s so long
	string type = "entity";
	LuaConfig::conf.getValue( "name", UnitName, type, defs->Name );
	LuaConfig::conf.getValue( "image", UnitName, type, defs->imageName );
	LuaConfig::conf.getValue( "height", UnitName, type, defs->height );
	LuaConfig::conf.getValue( "width", UnitName, type, defs->width );
	LuaConfig::conf.getValue( "imagecols", UnitName, type, defs->imagecols );
	LuaConfig::conf.getValue( "imagerows", UnitName, type, defs->imagerows );
    return true;
}

void Unit::setUnitType( enum e_unitID t_Unit )
{
        Type = t_Unit;        

    return;
}

void Unit::setUnitName( )
{
	UnitName = LuaConfig::conf.getRandom("meeting", "entity");
}

void Unit::setUnitX( float x )
{
        m_fX = x;
}

void Unit::setUnitY( float y )
{
    m_fY = y;
}

void Unit::setUnitAnim( int num )
{
    m_iAn = num;
}

bool Unit::setUnitImage( Texture* image)
{
	if( image != NULL ){
		m_Img = image;
		return true;
	}
	return false;
}

double Unit::dist( Unit* target )
{
	float x = m_fX - target->getUnitX();
	float y = m_fY - target->getUnitY();
    return sqrt( ( x * x ) + ( y * y ) );
}
