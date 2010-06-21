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

	//FIXME: where is my debug??
	defs = new EntityDefs;
	//FIXME: setdefault with unitname it's bad
	setUnitName();
	LuaConfig::conf.setDefault(UnitName);
	LuaConfig::conf.getValue( "name", defs->Name );
	LuaConfig::conf.getValue( "image", defs->imageName );
	LuaConfig::conf.getValue( "height", defs->height );
	LuaConfig::conf.getValue( "width", defs->width );
	LuaConfig::conf.getValue( "imagecols", defs->imagecols );
	LuaConfig::conf.getValue( "imagerows", defs->imagerows );
    return true;
}

void Unit::setUnitType( enum e_unitID t_Unit )
{
        Type = t_Unit;        

    return;
}

void Unit::setUnitName( )
{
	UnitName = LuaConfig::conf.getRandom("meeting");
	cout << UnitName.c_str() << endl;
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

bool Unit::setUnitImage( SDL_Surface* image)
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
