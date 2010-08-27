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
	blood->fixed = false;
	return true;
}

void Corpse::update( const int& dt )
{
	Plant::update( dt );
	blood->setPosition( getUnitX() - getUnitWidth()/4 , getUnitY(), Z - 1 );
	if( !blood->coordinates ){
		getUnitImage( )->setPosition( 0, 0, Z - 1);
		blood->resize( getUnitWidth() * getUnitSize(), getUnitHeight() * getUnitSize() );
		blood->coordinates = Graphics::Instance( )->GetAnimation( getUnitName(), 1 );
	}
}
