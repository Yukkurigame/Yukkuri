/*
 * Widgets.h
 *
 *  Created on: 30.06.2010
 */

#ifndef WIDGETS_H_
#define WIDGETS_H_

#include "safestring.h"
#include "GraphicsTypes.h"
#include <vector>


enum wType { NONE = 0, BLANK, TEXT, BAR};

enum wAlign { LEFT = 1, CENTER, RIGHT };
enum wVAlign { TOP = 1, MIDDLE, BOTTOM };


class Widget
{
public:
	Widget();
	virtual ~Widget();

	virtual bool load( std::string id );

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
	bool bindValue( std::string* val );

	//FIXME: to many virtual funcs
	virtual void Update() {};

	virtual void toggleVisibility( );

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
	std::string* SBinded;

	std::string baseID;

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

#endif /* WIDGETS_H_ */
