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

	void setFontColor( int r, int g, int b );
	void setText( std::string text );
	void setTextPosition( float x, float y );
	float getTextX( ){ return TextX; }
	float getTextY( ){ return TextY; }

	void Update();

	void toggleVisibility( );

	bool setText( lua_State* L );

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
