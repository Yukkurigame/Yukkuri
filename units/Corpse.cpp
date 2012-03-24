/*
 * Corpse.cpp
 *
 *  Created on: 23.08.2010
 */

#include "Corpse.h"
#include "Render.h"

Corpse::Corpse( )
{
	blood = NULL;
}

Corpse::~Corpse( )
{
	RenderManager::Instance( )->FreeGLSprite( blood );
}

bool Corpse::Create( int id )
{
	if( !Plant::Create( id ) )
		return false;
	/*if( getUnitImageCols() > 1 || getUnitImageRows() > 1 ){
		blood = RenderManager::Instance( )->CreateGLSprite( getUnitImageName( ) );
		blood->fixed = false;
	}*/
	return true;
}

void Corpse::update( const int& dt )
{
	Plant::update( dt );
	/*if( blood ){
		blood->setPosition( getUnitX() - getUnitWidth()/4 , getUnitY(), Z - 1 );
		if( !blood->coordinates ){
			getUnitImage( )->setPosition( 0, 0, Z - 1);
			blood->resize( getUnitWidth() * getUnitSize(), getUnitHeight() * getUnitSize() );
			blood->coordinates = Graphics::Instance( )->GetAnimation( getUnitName(), 1 );
		}
	}*/
}
