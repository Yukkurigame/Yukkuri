#include "Dynamic.h"
#include "daytime.h"


DynamicUnit::DynamicUnit()
{
	TotalDistance = 0;
}


bool DynamicUnit::loadAnimation()
{
	std::map < string, std::vector <int> > AnimTmp;
	//FIXME: it's so long
	LuaConfig::Instance()->getValue( "animation", UnitName, "entity", AnimTmp );
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
		//	speed = abs(self.dfn.speed * self.fed * 2) #(self.dfn.speed * self.world.map.speed(self.x, self.y))
		//float speed = 80.0f * ( dt / 1000.0f);
		float speed = abs( stat.speed * stat.fed ) * ( dt / 1000.0f ) / l;
		if( speed < 0.05f )
			speed = 0.05f;
		float dx = speed * x;// / l;
		float dy = speed * y;// / l;
		float distance = sqrt( dx * dx + dy * dy );
		TotalDistance += distance;
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
		setUnitAnim( anim[0] + ( anim[1] > 1 ? ( static_cast<int>(TotalDistance) / m_animdistance) % anim[1] : 0 ) );
		m_fX += dx;
		m_fY += dy;
		if( stat.fed > 0.01 )
			stat.fed -= distance / 10000;
	}
}

void DynamicUnit::grow( )
{
	stat.days++;
	stat.exp += 5;
}

void DynamicUnit::setFirstFrame( )
{
	if( anim )
		setUnitAnim( anim[0] );
}
