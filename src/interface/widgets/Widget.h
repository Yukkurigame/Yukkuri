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

	bool create( std::string id );

	virtual bool load( std::string id );

	void setType( wType t ){ Type = t; }

	void resize( float w, float h );
	virtual void updatePosition( );

	void setWidgetRealZ( float z ){ PosZ = z * 0.1f; }
	float getZ( );

	void setId( unsigned int id ) { ID = id; }

	virtual void setParent( Widget* p );
	inline Widget* getParent( ) { return Parent; }

	Widget* getChild( std::string name );
	void addChild( Widget* child );
	void removeChild( Widget* child );

	bool bindValue( enum type_identifier type, void* val );

	void setBackground( int texture, int picture );
	inline void setBackgroundColor( const s4ub& c ){
		if( background != NULL ){
			background->brush.set_color( c );
		}
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
	inline int getChildrenCount() { return Children.count( ); }


#define GET_PARAM( type, name, value )		\
	inline type getWidget##name() { return value; }
#define SET_PARAM( type, name, value )	\
	inline void setWidget##name( type _var ) { value = _var; }
#define GET_SET_PARAM( type, name, value )	\
	GET_PARAM( type, name, value )			\
	SET_PARAM( type, name, value )

	GET_PARAM( unsigned int, Id, ID )
	GET_PARAM( int, Type, Type )
	GET_PARAM( float, Width, Width )
	GET_PARAM( float, Height, Height )
	GET_PARAM( float, RealX, PosX )
	GET_PARAM( float, RealY, PosY )
	//GET_PARAM( float, RealZ, PosZ )
	//SET_PARAM( float, RealZ, PosZ )
	//GET_SET_PARAM( std::string, Name, Name )
	GET_SET_PARAM( float, X, OffsetX )
	GET_SET_PARAM( float, Y, OffsetY )

	std::string getWidgetName() { return Name; }
	void setWidgetName( std::string _var ) { Name = _var; }

#undef GET_PARAM
#undef SET_PARAM
#undef GET_SET_PARAM



protected:
	virtual CUData* createUData();

	bool visible;
	Sprite* background;

	float Width;
	float Height;
	float PosX;
	float PosY;
	float OffsetX;
	float OffsetY;

	std::string baseID;
	std::string Name;

	struct {
		void* ptr;
		enum type_identifier type;
	} Binded;


	WCaller Timer;

private:
	unsigned int ID;
	wType Type;
	// TODO: Aligns must be one flag
	int Align;
	//int VAlign; //FIXME: two align is bad?
	float PosZ;
	Widget* Parent;
	list<Widget*> Children;

};

#endif /* WIDGETS_H_ */
