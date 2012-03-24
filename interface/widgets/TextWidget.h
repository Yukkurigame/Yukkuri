/*
 * TextWidget.h
 *
 *  Created on: 24.03.2012
 *
 */

#ifndef TEXTWIDGET_H_
#define TEXTWIDGET_H_

#include "widgets/Widget.h"

class TextWidget: public Widget
{
public:
	TextWidget( );
	~TextWidget( );

	bool load( std::string config );

	void updatePosition( );

	void setFont( std::string name, int size ){ FontName = name; FontSize = size; }
	void setFontColor( int r, int g, int b );
	void setText( std::string text );
	void setTextPosition( float x, float y );
	float getTextX( ){ return TextX; }
	float getTextY( ){ return TextY; }

	void Update();

	void toggleVisibility( );

private:
	Sprite* TextSprite;
	float TextX, TextY;
	int TextAlign;
	std::string Text;
	std::string FontName;
	int FontSize;
	float BindedCache;
};


#endif /* TEXTWIDGET_H_ */
