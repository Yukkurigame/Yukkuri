#include "unit.h"

Unit::Unit()
{
    m_iAn = 0;
    m_fX = 0.0;
    m_fY = 0.0;
}

Unit::~Unit()
{
}

bool Unit::Create( string name )
{
  //setUnitName(name);
  defs = Config::conf.FindEntity(name);
  if (!defs)
      return false;
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
