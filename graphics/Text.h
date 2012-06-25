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


void CleanFonts();

class Text
{
public:
	Text();
	~Text();

	inline int width(){ return Width; }
	inline int height(){ return Height; }
	inline bool isVisible(){ return Visible; }
	inline bool isFixed(){ return Fixed; }
	inline int getLines( ){ return Lines; }
	inline int getLineHeight( ){ return lineHeight; }
	inline int getLineSize( ){ return font->cellHeight * lineHeight; }

	void setFont( std::string name, int size );
	void setPosition( float x, float y, float z );
	void setText( const char* text );
	void setColor( int r, int g, int b );
	void setLineHeight( float lh ){ lineHeight = lh; }

	void setVisible( bool v );
	void setFixed( bool f );

	void clear( );

private:
	int Width;
	int Height;
	int Lines;
	bool Fixed;
	bool Visible;
	s3f position;
	color4u color;
	font_data* font;

	float lineHeight;
	std::vector< Sprite* > sprites;

	void addSprite( int x, int y, Char* c );
};


#endif /* TEXT_H_ */
