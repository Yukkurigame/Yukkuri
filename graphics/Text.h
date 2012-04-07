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
	~Text();

	inline int width(){ return Width; }
	inline int height(){ return Height; }
	inline bool visible(){ return Visible; }
	inline bool fixed(){ return Fixed; }

	void setFont( std::string name, int size );
	void setPosition( float x, float y, float z );
	void setText( const char* text );
	void setColor( int r, int g, int b );

	void setVisible( bool v );
	void setFixed( bool f );

	void clear( );

private:
	int Width;
	int Height;
	bool Fixed;
	bool Visible;
	s3f position;
	color4u color;
	font_data* font;

	int lineHeight;
	std::vector< Sprite* > sprites;

	void addSprite( int x, int y, Char* c );
};


#endif /* TEXT_H_ */
