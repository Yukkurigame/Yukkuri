/*
 * Animation.h
 *
 *  Created on: 29.03.2012
 *
 */

#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <string>
#include "graphics/sprite/Sprite.h"


struct AnimationFrames
{
	unsigned int start;
	unsigned int end;
	AnimationFrames() : start(), end() {}
};

class Animation
{
public:
	Animation( );
	~Animation( );

	bool init( std::string subconfig, std::string config );

	void setAnimation( std::string );
	void setAnimation( unsigned int );
	void setPicture( int pic );
	void setFrame( int frame );
	void setDefault( );

	static int getAnimationId( std::string );

	inline int getPicture( ) { return picture; }

	void setPosition( float x, float y );
	void setPosition( float x, float y, float z );

	void setDepth( float z );
	inline float getDepth( ){ return sprite->brush.vertex_origin.z; }

	void setSize( float size );
	inline float getSize( ) { return size; }

	inline int getWidth( ) { return width; }
	inline int getHeight( ) { return height; }

	inline Sprite* getSprite(){ return sprite; }
	inline int getCount( ) { return count; }

private:
	int width;
	int height;
	int picture; // default frame
	int count;
	float size;
	Sprite* sprite;
	unsigned int currentId;
	unsigned int animationSize;
	AnimationFrames* animation; // Array
	AnimationFrames* current; // Pointer
};



#endif /* ANIMATION_H_ */
