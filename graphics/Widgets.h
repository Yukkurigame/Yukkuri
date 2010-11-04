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

enum wAlign { LEFT = 1, CENTER, RIGHT };
enum wVAlign { TOP = 1, BOTTOM = 3 };

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

	void resize( float w, float h );
	virtual void updatePosition( );

	void setZ( float z ){ PosZ = z * 0.1; }
	float getZ( );

	unsigned int getId() { return ID; }
	void setId( unsigned int id ) {  ID = id; }

	virtual void setParent( Widget* p );

	int childrenCount() { return Children.size( ); }
	void getChildren( Widget* children[], int size );
	void addChild( Widget* child );

	bool bindValue( float* val );

	//FIXME: to many virtual funcs
	virtual void Update() {};

	virtual void toggleVisibility( );

	virtual void setFont( string, int ) {};
	virtual void setFontColor( int r, int g, int b ) {};
	virtual void setText( string ) {};
	virtual void setTextPosition( float, float ) {};

	virtual void createBar( string name, int* ) {};
	virtual void setBarSize( float size ) {};
	virtual void setBarValue( int val ) {};

protected:
	bool visible;
	Graphics* graph;
	Sprite* background;

	float PosX;
	float PosY;
	float OffsetX;
	float OffsetY;
	float Width;
	float Height;

	float* Binded;

private:
	unsigned int ID;
	string Name;
	wType Type;
	int Align;
	int VAlign; //FIXME: two align is bad?
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

	void updatePosition( );

	void setFont( string name, int size ){ FontName = name; FontSize = size; }
	void setFontColor( int r, int g, int b );
	void setText( string text );
	void setTextPosition( float x, float y );
	float getTextX( ){ return TextX; }
	float getTextY( ){ return TextY; }

	void Update();

	void toggleVisibility( );

protected:
	string AddText;

private:
	Sprite* StaticTextSprite;
	Sprite* TextSprite;
	float TextX, TextY;
	int TextAlign;
	string Text;
	string FontName;
	int FontSize;

	float BindedCache;
};

class BarWidget: public TextWidget
{
public:
	BarWidget( );
	~BarWidget( );

	bool load( string config );

	void createBar( string name, int* position );
	void setBarValue( float val );
	void setBarSize( float val );

	void updatePosition( );

	bool bindBarMaxValue( float* val );

	void Update();

	void toggleVisibility( );

protected:
	Sprite* BarSprite;
	Sprite* TopSprite;

private:
	float BarX, BarY;
	float BarValue;
	float BarMaxValue; // in units
	float BarWidth; // in pixels

	float* BindedMaxValue;
};


#endif /* WIDGETS_H_ */
