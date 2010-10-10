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

enum wType { NONE = 0, BLANK, TEXT, BAR};

class Widget
{
public:
	Widget();
	virtual ~Widget();

	virtual bool create( string name, string text, int x=0, int y=0 );
	virtual bool load( string config );

	void setType( wType t ){ Type = t; }
	wType getType( ){ return Type; }

	string getName( ){ return Name; }

	float getX( ){ return PosX; }
	float getY( ){ return PosY; }
	float getHeight( ){ return Height; }
	float getWidth( ){ return Width; }

	void setZ( float z ){ PosZ = z * 0.1; }
	float getZ( );

	unsigned int getId() { return ID; }
	void setId( unsigned int id ) {  ID = id; }

	virtual void setParent( Widget* p );

	int childrenCount() { return Children.size( ); }
	void getChildren( Widget* children[], int size );
	void addChild( Widget* child );

	bool bindValue( float* val );

	virtual void toggleVisibility( );

	virtual void Update() {};

	virtual void setFont( string, int ) {};
	virtual void setFontColor( int r, int g, int b ) {};
	virtual void setText( string ) {};
	virtual void setTextPosition( float, float ) {};

	virtual void createBar( string name, int* ) {};
	virtual void setBarSize( int size ) {};
	virtual void setBarValue( int val ) {};

protected:
	bool visible;
	Graphics* graph;
	Sprite* background;

	float PosX;
	float PosY;
	float Width;
	float Height;

	float* Binded;

private:
	unsigned int ID;
	string Name;
	wType Type;
	float PosZ;
	Widget* Parent;
	std::vector<Widget*> Children;
};

class TextWidget: public Widget
{
public:
	TextWidget( );
	~TextWidget( );

	bool create( string name, string text, int x=0, int y=0 );
	bool load( string config );

	void setParent( Widget* p );

	void setFont( string name, int size ){ FontName = name; FontSize = size; }
	void setFontColor( int r, int g, int b );
	void setText( string text );
	void setTextPosition( float x, float y );
	float getTextX( ){ return textx; }
	float getTextY( ){ return texty; }

	void toggleVisibility( );

protected:
	string AddText;

private:

	void textPosition( float x, float y );

	Sprite* StaticTextSprite;
	Sprite* TextSprite;
	float textx, texty;
	string Text;
	string FontName;
	int FontSize;
};

class BarWidget: public TextWidget
{
public:
	BarWidget( );
	~BarWidget( );

	bool load( string config );

	void createBar( string name, int* position );
	void setBarValue( int size );
	void setBarSize( int val ){
		if( val > 0 )
			barmaxvalue = val;
		else
			barmaxvalue = 1;
	}

	void setParent( Widget* p );

	void Update();

	void toggleVisibility( );

protected:
	Sprite* bar;
	int barcolor[3];
	Sprite* top;

private:
	int barvalue;
	int barmaxvalue; // in units
	float barstartx;
	float barwidth; // in pixels
};


#endif /* WIDGETS_H_ */
