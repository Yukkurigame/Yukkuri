#include "Dynamic.h"

/** Move the unit according to the eight cardinal directions.
    @return Boolean value represents a Collision (true)
    @remark 
**/
void DynamicUnit::moveUnit( signed int x, signed int y, const int& dt )
{
    if( x != 0 || y != 0 )
    {
        float l = sqrt( x * x  +  y * y );
        //    speed = abs(self.dfn.speed * self.fed * 2) #(self.dfn.speed * self.world.map.speed(self.x, self.y))
        float speed = 80.0f * ( dt / 1000.0f);
        if( speed < 0.05f ) 
            speed = 0.05f;
        float dx = speed * x / l;
        float dy = speed * y / l;
        m_fdistance += sqrt( dx * dx + dy * dy );
        int anim; //FUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
        if( dx < 0 && dy < 0 ) //UUUUUUUUUUUUUUUUUUUUUU
            anim = 16; //UUUUUUUUUUUUUUUUUUUUUU
        else if( dx < 0 && dy > 0 ) //UUUUUUUUUUUUUUUUUUUUUU
            anim = 8; //UUUUUUUUUUUUUUUUUUUUUU
        else if( dx > 0 && dy < 0 ) //UUUUUUUUUUUUUUUUUUUUUU
            anim = 28; //UUUUUUUUUUUUUUUUUUUUUU
        else if( dx > 0 && dy > 0 ) //UUUUUUUUUUUUUUUUUUUUUU
            anim = 20; //UUUUUUUUUUUUUUUUUUUUUU
        else if( dy < 0 ) //UUUUUUUUUUUUUUUUUUUUUU
            anim = 4; //UUUUUUUUUUUUUUUUUUUUUU
        else if( dy > 0 ) //UUUUUUUUUUUUUUUUUUUUUU
            anim = 0; //UUUUUUUUUUUUUUUUUUUUUU
        else if( dx < 0 ) //UUUUUUUUUUUUUUUUUUUUUU
            anim = 12; //UUUUUUUUUUUUUUUUUUUUUU
        else if( dx > 0 ) //UUUUUUUUUUUUUUUUUUUUUU 
            anim = 24; //UUUUUUUUUUUUUUUUUUUUUU
        setUnitAnim( anim +((const int)(m_fdistance / m_animdistance) % 4));
        m_fX += dx;
        m_fY += dy;        
    }
}
