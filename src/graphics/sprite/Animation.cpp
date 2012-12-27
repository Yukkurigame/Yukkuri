/*
 * Animation.cpp
 *
 *  Created on: 29.03.2012
 *
 */
#include "graphics/sprite/Animation.h"
#include "graphics/Render.h"
#include "graphics/utils/gl_shader.h"
#include "scripts/LuaConfig.h"

#include "hacks.h"

#include <map>

namespace {

	std::map < std::string, unsigned int > animation_names;

}


Animation::Animation( )
{
	sprite = NULL;
	size = 1;
	width = height = picture = count = 0;
	animationSize = currentId = 0;
	animation = current = NULL;
}

Animation::~Animation( )
{
	RenderManager::FreeGLSprite(sprite);
	if( animation )
		free( animation );
}


bool Animation::init(  std::string subconfig, std::string config)
{
	std::map < std::string, std::pair <int, int> > anim;
	LuaConfig* cfg = new LuaConfig;

	std::string image;
	cfg->getValue( "height", subconfig, config, height );
	cfg->getValue( "width", subconfig, config, width );
	cfg->getValue( "image", subconfig, config, image );
	cfg->getValue( "picture", subconfig, config, picture );
	cfg->getValue( "animation", subconfig, config, anim );

	delete cfg;

	FOREACHIT( anim ){
		std::map < std::string, unsigned int >::iterator name = animation_names.find( it->first );
		if( name == animation_names.end() ){
			name = animation_names.insert(
					std::pair< std::string, int >( it->first, animation_names.size() + 1 )
					).first;
		}
		GROW_ARRAY( animation, AnimationFrames, name->second, animationSize )
		animation[name->second].start = it->second.first;
		animation[name->second].end = it->second.second;
	}

	sprite = RenderManager::CreateGLSprite( 0, 0, 0, width, height,
					RenderManager::GetTextureNumberById( image ), picture , 1 );
	//sprite->setCentered();
	sprite->setPicture( picture );
	sprite->shader = Shaders::getProgram( "lighting" );
	count = 0;



	return true;
}


void Animation::setAnimation( std::string name )
{
	setAnimation(getAnimationId( name ));
}

void Animation::setAnimation( unsigned int id )
{
	if( id == currentId )
		return;
	currentId = id;
	count = 0;
	current = NULL;
	if( id < animationSize ){
		current = &animation[id];
		count = current->end - current->start + 1;
	}
}

void Animation::setPicture( int pic )
{
	sprite->setPicture( pic );
}

void Animation::setFrame( int frame )
{
	if( count <= 0 || frame < 0 || frame > count )
		return setDefault();
	setPicture( current->start + frame );
}

void Animation::setDefault()
{
	setPicture(picture);
}

int Animation::getAnimationId( std::string name )
{
	std::map < std::string, unsigned int >::iterator n = animation_names.find( name );
	if( n != animation_names.end() )
		return n->second;
	animation_names.insert(
		std::pair< std::string, int >( name, animation_names.size() + 1 ) );
	return animation_names.size();
}


void Animation::setPosition( float x, float y )
{
	sprite->setPosition( x, y );
}

void Animation::setPosition( float x, float y, float z )
{
	sprite->setPosition( x, y, z );
}

void Animation::setDepth( float z )
{
	s3f vo = sprite->brush.vertex_origin;
	sprite->setPosition( vo.x, vo.y, z );
}

void Animation::setSize( float sz )
{
	size = sz;
	sprite->resize( width * size, height * size );
}

