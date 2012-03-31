/*
 * Animation.cpp
 *
 *  Created on: 29.03.2012
 *
 */
#include "Animation.h"
#include "Render.h"
#include "Luaconfig.h"

#include <vector>

Animation::Animation( )
{
	sprite = NULL;
}

Animation::~Animation( )
{
	RenderManager::Instance()->FreeGLSprite(sprite);
}


bool Animation::init(  std::string subconfig, std::string config)
{
	LuaConfig* cfg = new LuaConfig;

	std::string image;
	cfg->getValue( "height", subconfig, config, height );
	cfg->getValue( "width", subconfig, config, width );
	cfg->getValue( "image", subconfig, config, image );
	cfg->getValue( "picture", subconfig, config, picture );
	cfg->getValue( "animation", subconfig, config, animation );
	sprite = RenderManager::Instance()->CreateGLSprite( 0, 0, 0, width, height,
							RenderManager::Instance()->GetTextureById( image ), picture);
	sprite->centered = true;
	sprite->setPicture( picture );
	count = 0;

	delete cfg;

	return true;
}


void Animation::setAnimation( std::string name )
{
	current = animation.find(name);
	if( current == animation.end() )
		count = 0;
	else
		count = current->second.second - current->second.first + 1;
}

void Animation::setPicture( int pic )
{
	sprite->setPicture( pic );
}

void Animation::setFrame( int frame )
{
	if( count <= 0 || frame < 0 || frame > count )
		return setDefault();
	setPicture( current->second.first + frame );
}

void Animation::setDefault()
{
	setPicture(picture);
}

void Animation::setPosition( float x, float y )
{
	sprite->setPosition( x, y );
}

void Animation::setPosition( float x, float y, float z )
{
	sprite->setPosition( x, y, z );
}

void Animation::setSize( float sz )
{
	size = sz;
	sprite->resize( width * size, height * size );
}

