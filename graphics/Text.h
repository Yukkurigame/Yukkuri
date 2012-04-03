/*
 * Text.h
 *
 *  Created on: 31.03.2012
 *
 */

#ifndef TEXT_H_
#define TEXT_H_

#include <vector>
#include "Font.h"
#include "GraphicsTypes.h"

class Text
{
public:
	Text();

	void setFont( std::string name, int size );
	void setPosition( float x, float y, float z );
	void setText( const char* text );

	inline void setFixed( bool f ){ fixed = f; }

private:
	s3f position;
	font_data* font;
	bool fixed;
	std::vector< Sprite* > sprites;

	void addSprite( int x, int y, Char* c );
	int getTexture( Char* c );
};


#endif /* TEXT_H_ */
