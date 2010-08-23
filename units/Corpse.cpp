/*
 * Corpse.cpp
 *
 *  Created on: 23.08.2010
 */

#include "Corpse.h"
#include "Graphics.h"

Corpse::Corpse( )
{
	blood = NULL;
}

Corpse::~Corpse( )
{
	Graphics::Instance( )->FreeGLSprite( blood );
}

bool Corpse::Create( )
{
	if( !Plant::Create( ) )
		return false;
	blood = Graphics::Instance( )->CreateGLSprite( getUnitImageName( ) );
	blood->resize( getUnitWidth(), getUnitHeight() );
	blood->fixed = false;
	return true;
}

void Corpse::update( const int& dt )
{
	Plant::update( dt );
	blood->setPosition( getUnitX() - getUnitWidth()/4 , getUnitY() );
	if( !blood->coordinates ){
		blood->coordinates = Graphics::Instance( )->GetAnimation( getUnitName(), 1 );
	}
}
