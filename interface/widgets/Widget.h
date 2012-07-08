/*
 * Widgets.h
 *
 *  Created on: 30.06.2010
 */

#ifndef WIDGETS_H_
#define WIDGETS_H_

#include "GraphicsTypes.h"
#include <vector>

#include "3rdparty/CUDataUser.h"

class CUData;
struct LuaRet;


enum wType { wtNone = 0, wtBlank, wtText, wtBar, wtConsole };

enum wAlign { LEFT = 1, CENTER, RIGHT };
enum wVAlign { TOP = 1, MIDDLE, BOTTOM };


class Widget : public CUDataUser
{
public:
	Widget();
	virtual ~Widget();

	virtual bool load( std::string id );

	void setType( wType t ){ Type = t; }

	inline wType getType( ){ return Type; }
	inline std::string getName( ){ return Name; }

	float getX( ){ return PosX; }
	float getY( ){ return PosY; }
	int getHeight( ){ return Height; }
	int getWidth( ){ return Width; }

	void resize( float w, float h );
	virtual void updatePosition( );

	void setZ( float z ){ PosZ = z * 0.1f; }
	float getZ( );

	inline unsigned int getId() { return ID; }
	void setId( unsigned int id ) { ID = id; }

	virtual void setParent( Widget* p );
	inline Widget* getParent( ) { return Parent; }

	inline int childrenCount() { return static_cast<int>(Children.size( )); }

	Widget* getChildren( std::string name );
	LuaRet getChildren( lua_State* L );
	void addChild( Widget* child );

	bool bindValue( int* val );
	bool bindValue( float* val );
	bool bindValue( std::string* val );


	//FIXME: too many virtual funcs
	virtual void Update() {};

	bool getVisible( ) { return visible; };
	virtual void toggleVisibility( );

	virtual void setText( std::string ) {};
	virtual void setTextPosition( float, float ) {};

	// Lua methods
	bool resize( lua_State* L );
	bool toggle( lua_State* L );
	bool bindParam( lua_State* L );


protected:
	virtual CUData* createUData();

	bool visible;
	Sprite* background;

	int Width;
	int Height;
	float PosX;
	float PosY;
	float OffsetX;
	float OffsetY;

	int* iBinded;
	float* fBinded;
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
