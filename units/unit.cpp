#include "unit.h"

Unit::Unit()
{
	Anim = 0;
	X = 0.0;
	Y = 0.0;
	Scale = 1.0;
	UnitName = "";
	Type = "";
	Deleted = false;
}

Unit::~Unit()
{
	//delete Image;
	delete defs;
}

bool Unit::Create( )
{

	//string type = "entity";

	setUnitName( Type );

	//FIXME: where is my debug??
	defs = new EntityDefs;
	//FIXME: It,s so long
	getConfigValue( "name", defs->Name );
	if( defs->Name != UnitName )
		UnitName = defs->Name;
	getConfigValue( "image", defs->imageName );
	getConfigValue( "height", defs->height );
	getConfigValue( "width", defs->width );
	getConfigValue( "imagecols", defs->imagecols );
	getConfigValue( "imagerows", defs->imagerows );
	getConfigValue( "imgx", defs->imgoffsetx );
	getConfigValue( "imgy", defs->imgoffsety );

	return true;
}

void Unit::Delete( )
{
	Deleted = true;
}

void Unit::setUnitType( enum e_unitID type )
{
	switch( type ){
		case PLANT:
			Type = "plant";
			break;
		case CORPSE:
			Type = "corpse";
			break;
		default:
			Type = "entity";
			break;
	}
}

void Unit::setUnitName( string type )
{
	//FIXME: it's bad.
	UnitName = LuaConfig::Instance()->getRandom("meeting", type);
}

void Unit::setUnitX( float x )
{
	X = x;
}

void Unit::setUnitY( float y )
{
	Y = y;
}

void Unit::setUnitSize( float size )
{
	Scale = size;
	Image->resize( defs->width * Scale, defs->height * Scale );
}

void Unit::setUnitAnim( int num )
{
	Anim = num;
}

bool Unit::setUnitImage( Sprite* image)
{
	if( image != NULL ){
		Image = image;
		Image->centered = true;
		Image->fixed = false;
		return true;
	}
	return false;
}

double Unit::dist( Unit* target )
{
	float x = X - target->getUnitX();
	float y = Y - target->getUnitY();
	return sqrt( ( x * x ) + ( y * y ) );
}
