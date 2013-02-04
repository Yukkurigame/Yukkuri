/*
 * Text.h
 *
 *  Created on: 31.03.2012
 *
 */

#ifndef TEXT_H_
#define TEXT_H_

#include "graphics/Font.h"
#include "graphics/sprite/Sprite.h"
#include "graphics/GraphicsTypes.h"
#include "utils/list.h"


void CleanFonts();

class Text
{
public:
	Text();
	~Text();

	inline int width(){ return Width; }
	inline int height(){ return Height; }
	inline int getLines( ){ return Lines; }
	inline int getLineSize( ){ return (int)((float)font->cellHeight * lineHeight); }
	inline float getLineHeight( ){ return lineHeight; }

	void setFont( std::string name, int size );
	void setPosition( float x, float y, float z );
	void setText( const char* text );
	void setColor( int r, int g, int b );
	void setLineHeight( float lh ){
		if( !lh )
			lh = 1.0;
		lineHeight = lh;
	}

	void setVisible( bool v );
	inline unsigned char isVisible()	{ return flags & 1; }
	void setFixed( bool f );
	inline unsigned char isFixed()	{ return flags & 2; }

	inline unsigned char isCursorVisible()	{ if( cursor ) return cursor->isVisible(); return 0; }
	inline void setCursorVisible()			{ if( cursor ) cursor->setVisible(); }
	inline void clearCursorVisible()		{ if( cursor ) cursor->clearVisible(); }
	void setCursorPosition( unsigned int pos );
	inline int getCursorPosition() { return cursorPosition; }

	void clear( );

private:
	int Width;
	int Height;
	int Lines;
	unsigned int flags;		// 1 - visible
							// 2 - fixed
	float lineHeight;
	char* content;
	s4ub color;
	s3f position;
	font_data* font;

	int cursorBearing;
	int cursorPosition;
	Sprite* cursor;
	list< Sprite* > sprites;

	void addSprite( int x, int y, Char* c );
};


#endif /* TEXT_H_ */
