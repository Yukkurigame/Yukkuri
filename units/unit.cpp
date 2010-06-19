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
	string name = LuaConfig::conf.getRandom("meeting");
	cout << name.c_str() << endl;
	defs = new EntityDefs;
	LuaConfig::conf.setDefault(name);
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

void Unit::setUnitImage( SDL_Surface* image)
{
    m_Img = image;
}

double Unit::dist( Unit* target )
{
	float x = m_fX - target->getUnitX();
	float y = m_fY - target->getUnitY();
    return sqrt( ( x * x ) + ( y * y ) );
}
