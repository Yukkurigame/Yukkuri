/*
 * Corpse.cpp
 *
 *  Created on: 23.08.2010
 */

#include "units/UnitCorpse.h"
#include "graphics/Render.h"

Corpse::Corpse( )
{
	blood = NULL;
}

Corpse::~Corpse( )
{
	RenderManager::FreeGLSprite( blood );
}

bool Corpse::Create( int id, std::string proto )
{
	if( !UnitStatic::Create( id, proto ) )
		return false;
	/*
	TextureInfo* t = Image.getSprite()->tex;
	if( t->rows > 1 || t->cols > 1 ){
		blood = RenderManager::CreateGLSprite( Image.getSprite() );
		blood->setPicture( Image.getPicture() + 1 );
	}
	*/
	return true;
}

void Corpse::setUnitPos( float x, float y )
{
	UnitStatic::setUnitPos( x, y );
	if( blood ){
		blood->setPosition( getUnitX() - Image.getWidth() / 4, getUnitY(), Z - 0.1f );
		blood->resize( Image.getWidth() * getUnitSize(), Image.getHeight() * getUnitSize() );
	}
}
