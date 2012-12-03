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
	void setTextPosition( float x, float y );
	float getTextX( ){ return TextX; }
	float getTextY( ){ return TextY; }

	void Update();

	void toggleVisibility( );

	// Bindings
	inline int getLineHeight( ){ return (int)TextSprite.getLineHeight(); }
	inline void setLineHeight( float lh ){ TextSprite.setLineHeight( lh ); }
	inline int getLineSize( ){ return TextSprite.getLineSize(); }
	inline int getLines( ){ return (int)(Height / TextSprite.getLineSize()); }

	inline void setCursor( unsigned int pos ) { TextSprite.setCursorPosition( pos ); }
	inline int getCursor( ) { return TextSprite.getCursorPosition(); };
	inline unsigned char isCursorVisible()	{ return TextSprite.isCursorVisible(); }
	inline void setCursorVisible()			{ TextSprite.setCursorVisible(); }
	inline void clearCursorVisible()		{ TextSprite.clearCursorVisible(); }

	void setWidgetText( std::string text );
	inline std::string getWidgetText( ) { return TextContent; }


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
	double BindedCache;

};


#endif /* TEXTWIDGET_H_ */
