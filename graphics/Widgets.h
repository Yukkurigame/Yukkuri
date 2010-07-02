/*
 * Widgets.h
 *
 *  Created on: 30.06.2010
 */

#ifndef WIDGETS_H_
#define WIDGETS_H_

#include <string>
using std::string;
#include <vector>

#include "Graphics.h"
#include "debug.h"

enum wType { NONE = 0, BLANK, TEXT};

class Widget
{
public:
	Widget();
	~Widget();

	virtual bool create( string );

	void setType( wType t ){ type = t; }
	wType getType( ){ return type; }

	void setParent( Widget* p ){ parent = p; }

	void addChild( Widget* child );

	virtual void draw();

	//FIXME: как-то не хорошо так публично.
	string name;
	float width;
	float height;
	float posx;
	float posy;
	bool visible;

	virtual void setFont( string, int ) {};
	virtual void setText( string ) {};
	virtual void setTextPosition( float, float ) {};

protected:
	wType type;
	Sprite* background;
private:
	Widget* parent;
	std::vector<Widget*> children;
};

class TextWidget: public Widget
{
public:
	TextWidget( );

	bool create( string name );

	void setFont( string name, int size ){ FontName = name; FontSize = size; }
	void setText( string text ){ Text = text; }
	void setTextPosition( float x, float y ){ textx = x; texty = y; }
	void draw( );

private:
	float textx, texty;
	string Text;
	string FontName;
	int FontSize;
};


#endif /* WIDGETS_H_ */
