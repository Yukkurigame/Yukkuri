#include "unit.h"

void Unit::setUnitType( enum e_unitID t_Unit )
{
        Type = t_Unit;

    return;
}

/** Move the unit according to the eight cardinal directions.
    @return Boolean value represents a Collision (true)
    @remark 
**/
void Unit::moveUnit( signed int x, signed int y )
{
    if( x != 0 )
        setUnitX( m_iX + x );
    if( y != 0 )
        setUnitY( m_iY + y );
    /*if( Type == CMP || Type == PLY ) {
        switch( t_Dir ) {
            case NORTH:
                break;
            case NOREA:
                break;
            case EAST:
                break;
            case SOUEA:
                break;
            case SOUTH:
                break;
            case SOUWE:
                break;
            case WEST:
                break;
            case NORWE:
                break;
            default: break;
        }
    }
    */
}

void Unit::setUnitX( int x )
{
//    if( !m_iX )
        m_iX = x;

    return;
}

void Unit::setUnitY( int y )
{
//    if( !m_iY )
        m_iY = y;

    return;
}

