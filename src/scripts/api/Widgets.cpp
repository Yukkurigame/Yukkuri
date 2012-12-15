/*
 * Widgets.cpp
 *
 *  Created on: 20.05.2012
 *
 */


#include "interface/widgets/Widget.h"
#include "interface/widgets/WidgetBar.h"
#include "units/unitmanager.h"
#include "basic_types.h"

#include "graphics/Render.h"

#include "scripts/LuaScript.h"

#include "hacks.h"

extern float currentFPS;

extern LuaScript* luaScript;


#define GET_VALUE( name, param )	\
	luaScript->getValueByName( L, name, param, true );


bool Widget::load( lua_State* L )
{
	luaL_argcheck( L, lua_istable( L, 1 ), 1, "Parameters table expected" );


	std::string align;
	std::string valign;

	GET_VALUE( "name", Name )
	GET_VALUE( "x", OffsetX )
	GET_VALUE( "y", OffsetY )
	GET_VALUE( "width", Width )
	GET_VALUE( "height", Height )
	GET_VALUE( "align", Align )

	float z = 0;
	GET_VALUE( "depth", z )
	setWidgetRealZ( z );

	updatePosition( );

	{
		std::string imgname;
		int picture = 0;
		s4ub bgcolor(0,0,0,0);
		GET_VALUE( "image", imgname )
		GET_VALUE( "picture", picture )
		GET_VALUE( "bgcolor", bgcolor )
		if( imgname != "" || bgcolor.a ){
			setBackground( RenderManager::GetTextureNumberById( imgname ), picture );
			if( bgcolor.a )
				setBackgroundColor( bgcolor );
		}
	}

	lua_pop( L, 1 );

	return true;
}


bool WidgetText::load( lua_State* L )
{
	luaL_argcheck( L, lua_istable( L, 1 ), 1, "Parameters table expected" );

	std::string font;
	std::string text;
	std::string talign;
	std::string tvalign;
	int fontsize = 12;
	float lineheight;
	s4ub vcolor(0,0,0,255);

	GET_VALUE( "text", BaseText )
	GET_VALUE( "textx", TextX )
	GET_VALUE( "texty", TextY )
	GET_VALUE( "textalign", TextAlign )
	GET_VALUE( "font", font )
	GET_VALUE( "fontsize", fontsize )
	GET_VALUE( "fontcolor", vcolor )
	GET_VALUE( "lineheight", lineheight )
	if( !lineheight )
		lineheight = 1.0;

	TextSprite.setPosition( TextX, TextX, getZ() );
	TextSprite.setFont( font, fontsize );
	TextSprite.setFixed( true );
	TextSprite.setLineHeight( lineheight );
	setWidgetText( "" );

	setFontColor( vcolor );

	return Widget::load( L );
}


bool WidgetBar::load( lua_State* L )
{
	std::string imgname;
	int picture;
	int barheight;
	s4ub color;

	GET_VALUE( "barheight", barheight )
	GET_VALUE( "barwidth", BarWidth )
	GET_VALUE( "barx", BarX )
	GET_VALUE( "bary", BarY )
	GET_VALUE( "topimage", imgname )
	GET_VALUE( "toppicture", picture )
	GET_VALUE( "barcoverx", TopX )
	GET_VALUE( "barcovery", TopY )
	GET_VALUE( "barcolor", color )

	if( BarWidth <= 0 )
		BarWidth = (float)Width;

	createBar( imgname, picture, barheight, color );
	updatePosition();


	return WidgetText::load( L );
}


#undef GET_VALUE



bool Widget::resize( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ), 1, "Width expected" );
	luaL_argcheck( L, lua_isnumber( L, 2 ), 2, "Height expected" );

	this->resize( (float)lua_tonumber( L, 1 ), (float)lua_tonumber( L, 2 ) );

	lua_pop( L, 2 );

	return true;
}

bool Widget::toggle( lua_State* L )
{
	this->toggleVisibility();
	return this->visible;
}


bool Widget::bindParam( lua_State* L )
{
	luaL_argcheck( L, lua_isnil( L, 1 ) || lua_isnumber( L, 1 ) || lua_isuserdata( L, 1 ), 1, "Object expected" );
	luaL_argcheck( L, lua_isnumber( L, 2 ), 2, "Parameter name expected" );

	Unit* u = NULL;
	bool core = false;
	int paramname;

	if( lua_isnil( L, 1 ) ){
		core = true;
	}else if( lua_isnumber( L, 1 ) ){
		UINT id = static_cast<UINT>( lua_tointeger(L, 1) );
		u = UnitManager::GetUnit( id );
	}else
		u = reinterpret_cast<Unit*>( lua_touserdata( L, 1 ) );

	paramname = lua_tointeger( L, 2 );

	lua_pop( L, 2 );

	if( paramname <= 0 ){
		Debug::debug( Debug::INTERFACE, "Bad bind parameter.\n" );
		return false;
	}

	if( core ){
		switch(paramname){
			case gbpFPS:
				this->bindValue( tiFloat, &(currentFPS) );
				break;
			default:
				Debug::debug( Debug::INTERFACE, "Bad bind parameter.\n" );
				break;
		}
		return true;
	}

	if( !u ){
		Debug::debug( Debug::INTERFACE, "Bind object not found.\n" );
	}else{
		void* param = NULL;
		enum type_identifier type = tiNone;
		// FIXME: binded values point to wrong memory when unit dies.
		if( paramname < uCharIntLast ){
			param = u->getUnitpParameter( (enum character)paramname );
			type = tiInt;
		}else{
			param = u->getUnitpParameter( (enum character_float)paramname );
			type = tiFloat;
		}
		this->bindValue( type, param );
		return true;
	}

	return false;
}


LuaRet Widget::getChildren( lua_State* L )
{

	lua_newtable( L );
	int top = lua_gettop( L );
	Widget* child = NULL;
	LuaRet r(1);

	listElement<Widget*>* l = Children.head;
	while( l != NULL ){
		child = l->data;
		lua_pushstring( L, child->getWidgetName().c_str() );
		lua_pushinteger( L, child->getWidgetId() );
		lua_settable( L, top );
		l = l->next;
	}
	return r;

}


bool WidgetBar::bindBarMaxValue( lua_State* L )
{
	luaL_argcheck( L, lua_isnumber( L, 1 ) || lua_isuserdata( L, 1 ), 1, "Object expected" );
	luaL_argcheck( L, lua_isstring( L, 2 ), 2, "Parameter name expected" );

	Unit* u = NULL;
	enum character_float paramname;

	if( lua_isnumber( L, 1 ) ){
		UINT id = static_cast<UINT>( lua_tointeger(L, 1) );
		u = UnitManager::GetUnit( id );
	}else
		u = reinterpret_cast<Unit*>( lua_touserdata( L, 1 ) );

	paramname = static_cast<enum character_float>( lua_tointeger( L, 2 ) );

	lua_pop( L, 2 );

	if( !u ){
		Debug::debug( Debug::INTERFACE, "Bind object not found.\n" );
	}else if( paramname == 0 ){
		Debug::debug( Debug::INTERFACE, "Bad bind parameter.\n" );
	}else{
		this->bindBarMaxValue( u->getUnitpParameter( paramname ) );
		return true;
	}

	return false;
}


