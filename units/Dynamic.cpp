#include "Dynamic.h"


DynamicUnit::DynamicUnit()
{
	loadAnimation();
}

bool DynamicUnit::loadAnimation()
{
	//Ололо, если брать возвращаемый бул, то у первого загружаемого юнита пропадает анимация. wtf?
	LuaConfig::conf.getValue( "animation", AnimTmp );
	return true;
}

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
        if( dx < 0 && dy < 0 ) 			//FUUUU
            anim = AnimTmp["leftup"];	//UUUUU
        else if( dx < 0 && dy > 0 ) 	//UUUUU
            anim = AnimTmp["leftdown"]; //UUUUU
        else if( dx > 0 && dy < 0 ) 	//UUUUU
            anim = AnimTmp["rightup"]; 	//UUUUU
        else if( dx > 0 && dy > 0 ) 	//UUUUU
            anim = AnimTmp["rightdown"];//UUUUU
        else if( dy < 0 ) 				//UUUUU
            anim = AnimTmp["up"]; 		//UUUUU
        else if( dy > 0 ) 				//UUUUU
            anim = AnimTmp["down"]; 	//UUUUU
        else if( dx < 0 ) 				//UUUUU
            anim = AnimTmp["left"]; 	//UUUUU
        else if( dx > 0 ) 				//UUUUU
            anim = AnimTmp["right"];	//UUUUU
        setUnitAnim( anim[0] + ( anim[1] > 1 ? (const int)(m_fdistance / m_animdistance) % anim[1] : 0 ));
        m_fX += dx;
        m_fY += dy;        
    }
}
