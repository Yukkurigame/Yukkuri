/*
 * Widgets.h
 *
 *  Created on: 30.06.2010
 */

#ifndef WIDGETS_H_
#define WIDGETS_H_

#include "basic_types.h"
#include "graphics/sprite/Sprite.h"
#include "3rdparty/CUDataUser.h"
#include "3rdparty/timer/ITimerEventPerformer.h"


class CUData;
struct LuaRet;


enum wType { wtNone = 0, wtBlank, wtText, wtBar, wtConsole };

enum wAlign { waMIDDLE=0, waLEFT = 1, waRIGHT=2, waTOP = 4, waBOTTOM = 8 };


#define UPDATE_PERIOD 200


class Widget : public CUDataUser
{
public:
	class WCaller : public ITimerEventPerformer
	{
		Widget* parent;
		int event_id;
	public:
		WCaller(Widget* t) : parent(t), event_id(-1) {}
		virtual ~WCaller(){ };

		void start( );
		void stop( );

		void OnTimer( InternalTimerEvent& ev ){
			parent->Update();
		}

		void OnTimerEventDestroy( const InternalTimerEvent& ev ){	}
	};

	Widget();
	virtual ~Widget();

	bool create( const char* name, wType t );

	void resize( float w, float h );
	virtual void redraw( );

	void setWidgetRealZ( float z ){ Position.z = z * 0.1f; }
	float getZ( );
	inline const rect2f& getRect(){ return Rect; }

	void setId( unsigned int id ) { ID = id; }

	virtual void setParent( Widget* p );
	inline Widget* getParent( ) { return Parent; }

	Widget* getChild( const char* name );
	void addChild( Widget* child );
	void removeChild( Widget* child );

	bool bindValue( enum type_identifier type, void* val );

	void setBackground( int texture, int picture );
	inline void setBackgroundColor( const s4ub& c ){
		if( background != NULL )
			background->brush.set_color( c );
	}

	//FIXME: too many virtual funcs
	virtual void Update() {};

	bool getVisible( ) { return visible; };
	virtual void toggleVisibility( );

	// Lua methods
	virtual bool load( lua_State* L );
	bool resize( lua_State* L );
	bool toggle( lua_State* L );
	bool bindParam( lua_State* L );
	LuaRet getChildren( lua_State* L );
	inline int getChildrenCount() { return Children.count; }


#define GET_PARAM( type, name, value )		\
	inline type getWidget##name() { return value; }
#define SET_PARAM( type, name, value )	\
	inline void setWidget##name( type _var ) { value = _var; }
#define GET_SET_PARAM( type, name, value )	\
	GET_PARAM( type, name, value )			\
	SET_PARAM( type, name, value )

	GET_PARAM( unsigned int, Id, ID )
	GET_PARAM( int, Type, Type )
	GET_PARAM( float, Width, Rect.width )
	GET_PARAM( float, Height, Rect.height )
	GET_PARAM( float, RealX, Position.x )
	GET_PARAM( float, RealY, Position.y )
	GET_PARAM( float, RealZ, Position.z )
	GET_PARAM( const char*, Name, Name )
	GET_PARAM( int, Picture, ( background ? background->picture : 0 ) )
	//SET_PARAM( float, RealZ, PosZ )
	//GET_SET_PARAM( std::string, Name, Name )
	GET_SET_PARAM( float, X, Rect.x )
	GET_SET_PARAM( float, Y, Rect.y )

	s3f getWidgetPosition(){ return s3f(Position.x, Position.y, getZ()); }
	inline void setWidgetPicture( int p ){
		if( background != NULL )
			background->setPicture( p );
	}

#undef GET_PARAM
#undef SET_PARAM
#undef GET_SET_PARAM


protected:
	virtual CUData* createUData();

	bool visible;
	Sprite* background;

	struct {
		void* ptr;
		enum type_identifier type;
	} Binded;

	WCaller Timer;

private:
	unsigned int ID;
	wType Type;
	const char* Name;

	int Align;

	s3f Position; 	// Real word position
	rect2f Rect;	// Screen position

	Widget* Parent;
	list<Widget*> Children;
};

#endif /* WIDGETS_H_ */
