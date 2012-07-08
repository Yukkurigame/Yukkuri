/*
 * TextWidget.h
 *
 *  Created on: 24.03.2012
 *
 */

#ifndef TEXTWIDGET_H_
#define TEXTWIDGET_H_

#include "interface/widgets/Widget.h"
#include "graphics/Text.h"

class WidgetText: public Widget
{
public:
	WidgetText( );
	~WidgetText( );

	bool load( std::string id );

	void updatePosition( );

	void setFontColor( unsigned int r, unsigned int g, unsigned int b );
	void setText( std::string text );
	void setTextPosition( float x, float y );
	float getTextX( ){ return TextX; }
	float getTextY( ){ return TextY; }

	inline float getLineHeight( ){ return TextSprite.getLineHeight(); }
	inline void  setLineHeight( float lh ){ TextSprite.setLineHeight( lh ); }

	inline int getLineSize( ){ return TextSprite.getLineSize(); }
	inline int getLines( ){ return Height / TextSprite.getLineSize(); }


	void Update();

	void toggleVisibility( );
	inline unsigned char isCursorVisible()	{ return TextSprite.isCursorVisible(); }
	inline void setCursorVisible()			{ TextSprite.setCursorVisible(); }
	inline void clearCursorVisible()		{ TextSprite.clearCursorVisible(); }

	bool setText( lua_State* L );

	inline void setCursor( unsigned int pos ) { TextSprite.setCursorPosition( pos ); }
	inline int getCursor( ) { return TextSprite.getCursorPosition(); };


protected:
	virtual CUData* createUData();

	std::string BaseText;

private:
	Text TextSprite;
	float TextX, TextY;
	int TextAlign, TextVAlign;
	std::string TextContent;
	std::string FontName;
	int FontSize;
	int iBindedCache;
	float fBindedCache;

};


#endif /* TEXTWIDGET_H_ */
