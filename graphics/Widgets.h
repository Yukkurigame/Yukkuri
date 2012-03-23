/*
 * Widgets.h
 *
 *  Created on: 30.06.2010
 */

#ifndef WIDGETS_H_
#define WIDGETS_H_

#include "safestring.h"
#include <vector>

#include "debug.h"

enum wType { NONE = 0, BLANK, TEXT, BAR};

enum wAlign { LEFT = 1, CENTER, RIGHT };
enum wVAlign { TOP = 1, BOTTOM = 3 };

class Widget
{
public:
	Widget();
	virtual ~Widget();

	virtual bool create( std::string name, std::string text, int x=0, int y=0 );
	virtual bool load( std::string config );

	void setType( wType t ){ Type = t; }
	wType getType( ){ return Type; }

	std::string getName( ){ return Name; }

	float getX( ){ return PosX; }
	float getY( ){ return PosY; }
	int getHeight( ){ return Height; }
	int getWidth( ){ return Width; }

	void resize( float w, float h );
	virtual void updatePosition( );

	void setZ( float z ){ PosZ = z * 0.1f; }
	float getZ( );

	unsigned int getId() { return ID; }
	void setId( unsigned int id ) {  ID = id; }

	virtual void setParent( Widget* p );

	int childrenCount() { return static_cast<int>(Children.size( )); }
	void getChildren( Widget* children[], int size );
	void addChild( Widget* child );

	bool bindValue( float* val );

	//FIXME: to many virtual funcs
	virtual void Update() {};

	virtual void toggleVisibility( );

	virtual void setFont( std::string, int ) {};
	virtual void setFontColor( int r, int g, int b ) {};
	virtual void setText( std::string ) {};
	virtual void setTextPosition( float, float ) {};

	virtual void createBar( std::string name, int* ) {};
	virtual void setBarSize( float size ) {};
	virtual void setBarValue( int val ) {};

protected:
	bool visible;
	Sprite* background;

	int Width;
	int Height;
	float PosX;
	float PosY;
	float OffsetX;
	float OffsetY;

	float* Binded;

private:
	unsigned int ID;
	std::string Name;
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

	bool create( std::string name, std::string text, int x=0, int y=0 );
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

protected:
	std::string AddText;

private:
	Sprite* StaticTextSprite;
	Sprite* TextSprite;
	float TextX, TextY;
	int TextAlign;
	std::string Text;
	std::string FontName;
	int FontSize;

	float BindedCache;
};

class BarWidget: public TextWidget
{
public:
	BarWidget( );
	~BarWidget( );

	bool load( std::string config );

	void createBar( std::string name, int* position );
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
