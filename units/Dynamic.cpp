#include "Dynamic.h"


DynamicUnit::DynamicUnit()
{
	TotalDistance = 0;
	anim = NULL;
	Attacked = NULL;
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
		float speed = fabs( stat.speed * stat.fed ) * ( dt / 1000.0f ) / l;
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
		if( stat.fed > 0.01 )
			stat.fed -= distance / 20000;
	}
}

void DynamicUnit::grow( )
{
	stat.days++;
	stat.exp += 10 * stat.level;
}

void DynamicUnit::eat( Unit* victim )
{
	//TODO: пересмотреть надо бы
	if( !victim->isEdible( ) )
		return;
	float dmg = stat.damage / stat.fed;
	victim->hit( dmg );
	if( stat.fed >= 0.8 && stat.fed < 1.0 && stat.hp < stat.hpMax ){
		float hpAdd = dmg / stat.level;
		if( (stat.hp + hpAdd) > stat.hpMax )
			stat.hp = stat.hpMax;
		else
			stat.hp += hpAdd;
	}
	if( stat.fed >= 1.0 ){
		stat.fed = 1.0;
		if( stat.hp < stat.hpMax ){
			float hpAdd = dmg;
			if( ( stat.hp + hpAdd ) > stat.hpMax )
				stat.hp = stat.hpMax;
			else
				stat.hp += hpAdd;
		}
	}else{
		float n = victim->getNutritive();
		float fedAdd = dmg * 0.01 * n / stat.level;
		if( ( stat.fed + fedAdd ) > 1.0 )
			stat.fed = 1.0;
		else
			stat.fed += fedAdd;
	}
}

void DynamicUnit::levelUp( int addlevel )
{
	//FIXME: level not decreased;
	float scale;
	float hp;
	for( int i = 1; i<= addlevel; ++i){
		stat.level++;
		scale = ( log( static_cast<float>(stat.level) ) / log( static_cast<float>(40) ) );
		if( scale < 0.35 )
			scale = 0.35;
		else if( scale > 1.3 )
			scale = 1.3;
		setUnitSize( scale );
		hp = stat.hp / stat.hpMax;
		stat.exp = stat.exp - stat.expMax;
		stat.hpMax += 10 * stat.level;
		stat.expMax += stat.expMax + log( static_cast<float>(stat.level) / static_cast<float>(40) );
		stat.hp = stat.hpMax * hp;
	}
}

void DynamicUnit::update( const int& dt )
{
	AnimatedUnit::update( dt );
	if( this->stat.exp >= this->stat.expMax )
		levelUp(1);
}

void DynamicUnit::takeAction( )
{
	AnimatedUnit::takeAction();
	if( stat.fed > 0.01 )
			stat.fed -= 0.002 * stat.level;
	if( Attacked ){
		if( Attacked->getUnitStats()->hp <= 0 || dist(Attacked) >= 1000 ){
			Attacked = NULL;
		}
	}
}

void DynamicUnit::attackUnit( Unit* victim )
{
	if( victim == this )
		return;
	if( stat.fed > 0.01 )
		stat.fed -= 0.001;
    float dmg =  stat.damage * stat.fed;
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
	stat.hp -= damage;
}

void DynamicUnit::setFirstFrame( )
{
	if( anim != NULL )
		setUnitAnim( anim[0] );
}
