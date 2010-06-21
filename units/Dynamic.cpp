#include "Dynamic.h"


DynamicUnit::DynamicUnit()
{

}


bool DynamicUnit::loadAnimation()
{
	std::map < string, std::vector <int> > AnimTmp;
	//FIXME: setdefault with unitname it's bad
	LuaConfig::conf.setDefault(UnitName);
	LuaConfig::conf.getValue( "animation", AnimTmp );
	copy(AnimTmp["down"].begin(), AnimTmp["down"].begin()+2, Animdef.down);
	copy(AnimTmp["leftdown"].begin(), AnimTmp["leftdown"].begin()+2, Animdef.leftdown);
	copy(AnimTmp["left"].begin(), AnimTmp["left"].begin()+2, Animdef.left);
	copy(AnimTmp["leftup"].begin(), AnimTmp["leftup"].begin()+2, Animdef.leftup);
	copy(AnimTmp["up"].begin(), AnimTmp["up"].begin()+2, Animdef.up);
	copy(AnimTmp["right"].begin(), AnimTmp["right"].begin()+2, Animdef.right);
	copy(AnimTmp["rightdown"].begin(), AnimTmp["rightdown"].begin()+2, Animdef.rightdown);
	copy(AnimTmp["rightup"].begin(), AnimTmp["rightup"].begin()+2, Animdef.rightup);
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
            anim = Animdef.leftup;		//UUUUU
        else if( dx < 0 && dy > 0 ) 	//UUUUU
            anim = Animdef.leftdown;	//UUUUU
        else if( dx > 0 && dy < 0 ) 	//UUUUU
            anim = Animdef.rightup; 	//UUUUU
        else if( dx > 0 && dy > 0 ) 	//UUUUU
            anim = Animdef.rightdown;	//UUUUU
        else if( dy < 0 ) 				//UUUUU
            anim = Animdef.up;	 		//UUUUU
        else if( dy > 0 ) 				//UUUUU
            anim = Animdef.down;	 	//UUUUU
        else if( dx < 0 ) 				//UUUUU
            anim = Animdef.left;	 	//UUUUU
        else if( dx > 0 ) 				//UUUUU
            anim = Animdef.right;		//UUUUU
        setUnitAnim( anim[0] + ( anim[1] > 1 ? (const int)(m_fdistance / m_animdistance) % anim[1] : 0 ));
        m_fX += dx;
        m_fY += dy;        
    }
}
