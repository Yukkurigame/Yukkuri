/*
 * Animation.h
 *
 *  Created on: 29.03.2012
 *
 */

#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <map>
#include <utility>
#include <string>
#include "GraphicsTypes.h"

class Animation
{
public:
	Animation( );
	~Animation( );

	bool init( std::string subconfig, std::string config );

	void setAnimation( std::string );
	void setPicture( int pic );
	void setFrame( int frame );
	void setDefault( );

	inline int getPicture( ) { return picture; }

	void setPosition( float x, float y );
	void setPosition( float x, float y, float z );

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
	std::map < std::string, std::pair <int, int> > animation;
	std::map < std::string, std::pair <int, int> >::iterator current;
};



#endif /* ANIMATION_H_ */
