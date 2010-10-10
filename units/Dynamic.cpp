#include "Dynamic.h"
#include <math.h>


DynamicUnit::DynamicUnit()
{
	TotalDistance = 0;
	anim = NULL;
	Attacked = NULL;
	Parameters["hp"] = 10;
	Parameters["hpmax"] = 10;
	Parameters["fed"] = 100;
	Parameters["exp"] = 20;
	Parameters["expmax"] = 200;
	Parameters["damage"] = 3;
	Parameters["level"] = 1;
	Parameters["speed"] = 80;

	//FIXME: get it from config
	FoodTypes.push_back( "plant" );
}


bool DynamicUnit::loadAnimation()
{
	std::map < string, std::vector <int> > AnimTmp;
	//FIXME: it's so long
	getConfigValue( "animation", AnimTmp );
	copy(AnimTmp["down"].begin(), AnimTmp["down"].begin()+2, Animdef.down);
	copy(AnimTmp["leftdown"].begin(), AnimTmp["leftdown"].begin()+2, Animdef.leftdown);
	copy(AnimTmp["left"].begin(), AnimTmp["left"].begin()+2, Animdef.left);
	copy(AnimTmp["leftup"].begin(), AnimTmp["leftup"].begin()+2, Animdef.leftup);
	copy(AnimTmp["up"].begin(), AnimTmp["up"].begin()+2, Animdef.up);
	copy(AnimTmp["right"].begin(), AnimTmp["right"].begin()+2, Animdef.right);
	copy(AnimTmp["rightdown"].begin(), AnimTmp["rightdown"].begin()+2, Animdef.rightdown);
	copy(AnimTmp["rightup"].begin(), AnimTmp["rightup"].begin()+2, Animdef.rightup);

	//FIXME: move from here
	setUnitSize(0.35);

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
		float speed = fabs( Parameters["speed"] * Parameters["fed"] ) * ( dt / 100000.0 ) / l;
		if( speed < 0.05f )
			speed = 0.05f;
		float dx = speed * x;// / l;
		float dy = speed * y;// / l;
		float distance = sqrt( dx * dx + dy * dy );
		TotalDistance += distance;
		if( dx < 0 && dy > 0 ) 			//FUUUU
			anim = Animdef.leftup;		//UUUUU
		else if( dx < 0 && dy < 0 ) 	//UUUUU
			anim = Animdef.leftdown;	//UUUUU
		else if( dx > 0 && dy > 0 ) 	//UUUUU
			anim = Animdef.rightup; 	//UUUUU
		else if( dx > 0 && dy < 0 ) 	//UUUUU
			anim = Animdef.rightdown;	//UUUUU
		else if( dy > 0 ) 				//UUUUU
			anim = Animdef.up;	 		//UUUUU
		else if( dy < 0 ) 				//UUUUU
			anim = Animdef.down;	 	//UUUUU
		else if( dx < 0 ) 				//UUUUU
			anim = Animdef.left;	 	//UUUUU
		else if( dx > 0 ) 				//UUUUU
			anim = Animdef.right;		//UUUUU
		setUnitAnim( anim[0] + ( anim[1] > 1 ? ( static_cast<int>(TotalDistance) / m_animdistance) % anim[1] : 0 ) );
		X += dx;
		Y += dy;
		if( Parameters["fed"] > 1 )
			Parameters["fed"] -= distance / 200;
	}
}

void DynamicUnit::grow( )
{
	++Parameters["days"];
	Parameters["exp"] += 10 * Parameters["level"];
}

void DynamicUnit::eat( Unit* victim )
{
	//TODO: пересмотреть надо бы
	if( !victim->isEdible( ) )
		return;
	float fed = Parameters["fed"];
	float level = Parameters["level"];
	float hp = Parameters["hp"];
	float hpmax = Parameters["hpmax"];
	float dmg = Parameters["damage"] / fed * 100;
	if( level == 0 ) level = 1;
	victim->hit( dmg );
	if( fed >= 80 && fed < 100 && hp < hpmax ){
		float hpAdd = dmg / level;
		if( (hp + hpAdd) > hpmax )
			Parameters["hp"] = hpmax;
		else
			Parameters["hp"] += hpAdd;
	}
	if( fed >= 100 ){
		Parameters["fed"] = 100;
		if( hp < hpmax ){
			float hpAdd = dmg;
			if( ( hp + hpAdd ) > hpmax )
				Parameters["hp"] = hpmax;
			else
				Parameters["hp"] += hpAdd;
		}
	}else{
		float n = victim->getUnitParameter("nutritive");
		float fedAdd = dmg * n / level;
		if( ( fed + fedAdd ) > 100 )
			Parameters["fed"] = 100;
		else
			Parameters["fed"] += fedAdd;
	}
}

void DynamicUnit::levelUp( int addlevel )
{
	//FIXME: level not decreased;
	float scale;
	float hp;
	float hpmax;
	float expmax;
	float level;
	for( int i = 1; i<= addlevel; ++i){
		level = ++Parameters["level"];
		scale = ( log( level ) / log( static_cast<float>(40) ) );
		if( scale < 0.35 )
			scale = 0.35;
		else if( scale > 1.3 )
			scale = 1.3;
		setUnitSize( scale );
		hpmax = Parameters["hpmax"];
		if( hpmax == 0 ) hpmax = 1;
		expmax = Parameters["expmax"];
		hp = Parameters["hp"] / hpmax;
		Parameters["exp"] -= expmax;
		Parameters["hpmax"] += 10 * level;
		Parameters["expmax"] = expmax + log( level / static_cast<float>(40) );
		Parameters["hp"] = Parameters["hpmax"] * hp;
	}
}

void DynamicUnit::update( const int& dt )
{
	AnimatedUnit::update( dt );
	if( this->Parameters["exp"] >= this->Parameters["expmax"] )
		levelUp(1);
}

void DynamicUnit::takeAction( )
{
	AnimatedUnit::takeAction();
	if( Parameters["fed"] > 1 )
		Parameters["fed"] -= 0.2 * Parameters["level"];
	if( Attacked ){
		if( Attacked->getUnitParameter( "hp" ) <= 0 || dist(Attacked) >= 1000 ){
			Attacked = NULL;
		}
	}
}

void DynamicUnit::attackUnit( Unit* victim )
{
	if( victim == this || !victim )
		return;
	if( Parameters["fed"] > 1 )
		Parameters["fed"] -= 0.1;
    float dmg =  Parameters["damage"] * Parameters["fed"] / 100;
    if( dmg < 0.4 )
        dmg = 0.4;
    DynamicUnit* dvictim = dynamic_cast<DynamicUnit*>(victim);
    if( dvictim && dvictim->Attacker() != this )
    	dvictim->Attacker( this );
	victim->hit( dmg );
}

void DynamicUnit::hit( float damage )
{
	this->getUnitImage()->clr->set( 255, 0, 0 );
	Parameters["hp"] -= damage;
}

void DynamicUnit::setFirstFrame( )
{
	if( anim != NULL )
		setUnitAnim( anim[0] );
}
