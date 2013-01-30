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

#include "graphics/render/Textures.h"

#include "scripts/LuaScript.h"

#include "hacks.h"

extern float currentFPS;

extern LuaScript* luaScript;

#define GET_VALUE( param )	\
	luaScript->getValue( L, -1, param );
#define GET_NAMED_VALUE( name, param )	\
	luaScript->getValueByName( L, name, param, true );
#define IS_TABLE( name )			\
	lua_getfield( L, -1, name );	\
	if( lua_istable( L, -1 ) )


bool Widget::load( lua_State* L )
{
	luaL_argcheck( L, lua_istable( L, 1 ), 1, "Parameters table expected" );

	std::string align;
	std::string valign;

	GET_VALUE( Rect );
	GET_NAMED_VALUE( "name", Name )
	GET_NAMED_VALUE( "align", Align )

	float z = 0;
	GET_NAMED_VALUE( "depth", z )
	setWidgetRealZ( z );

	{
		std::string imgname;
		int picture = 0;
		s4ub bgcolor(0,0,0,0);
		GET_NAMED_VALUE( "image", imgname )
		GET_NAMED_VALUE( "picture", picture )
		GET_NAMED_VALUE( "bgcolor", bgcolor )
		if( imgname != "" || bgcolor.a ){
			setBackground( Textures::get_by_name( imgname.c_str() ), picture );
			if( bgcolor.a )
				setBackgroundColor( bgcolor );
		}
	}

	lua_pop( L, 1 );

	redraw( );

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
	float lineheight = 1.0;
	s4ub vcolor( 0, 0, 0, 255 );

	IS_TABLE( "text" ){
		GET_NAMED_VALUE( "base", BaseText )
		GET_NAMED_VALUE( "x", TextPos.x )
		GET_NAMED_VALUE( "y", TextPos.y )
		GET_NAMED_VALUE( "align", TextAlign )
		GET_NAMED_VALUE( "face", font )
		GET_NAMED_VALUE( "size", fontsize )
		GET_NAMED_VALUE( "color", vcolor )
		GET_NAMED_VALUE( "lineheight", lineheight )
	}
	lua_pop(L, 1);

	/*GET_NAMED_VALUE( "text", BaseText )
	GET_NAMED_VALUE( "textx", TextPos.x )
	GET_NAMED_VALUE( "texty", TextPos.y )
	GET_NAMED_VALUE( "textalign", TextAlign )
	GET_NAMED_VALUE( "font", font )
	GET_NAMED_VALUE( "fontsize", fontsize )
	GET_NAMED_VALUE( "fontcolor", vcolor )
	GET_NAMED_VALUE( "lineheight", lineheight )
	*/

	TextSprite.setPosition( TextPos.x, TextPos.x, getZ() );
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
	s4ub color;
	int picture = 0;

	IS_TABLE( "bar" ){
		GET_VALUE( Bar );
		GET_NAMED_VALUE( "color", color )
	}
	lua_pop(L, 1);

	IS_TABLE( "top" ){
		GET_VALUE( Top );
		GET_NAMED_VALUE( "image", imgname )
		GET_NAMED_VALUE( "picture", picture )
	}
	lua_pop(L, 1);

	createBar( imgname, picture, color );

	return WidgetText::load( L );
}


#undef GET_NAMED_VALUE



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
		u = reinterpret_cast<Unit*>( getUserData( L, 1 ) );

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
		u = reinterpret_cast<Unit*>( getUserData( L, 1 ) );

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


