/*
 * TextWidget.cpp
 *
 *  Created on: 24.03.2012
 *
 */

#include "interface/widgets/WidgetText.h"
#include "graphics/Render.h"
#include "scripts/LuaConfig.h"

#include "safestring.h"


WidgetText::WidgetText( )
{
	FontName = "DejaVuSans";
	FontSize = 12;
	TextAlign = waMIDDLE;
	TextContent = "";
	BindedCache = 0.00123f;
}


WidgetText::~WidgetText( )
{

}


bool WidgetText::load( std::string id )
{
	std::string font;
	std::string text;
	std::string talign;
	std::string tvalign;
	int fontsize = 12;
	float lineheight;
	s4ub vcolor( 0,0,0,0 );
	LuaConfig* cfg = new LuaConfig;

	cfg->getValue( "text", id.c_str(), BaseText );
	cfg->getValue( "textx", id.c_str(), TextPos.x );
	cfg->getValue( "texty", id.c_str(), TextPos.y );
	cfg->getValue( "textalign", id.c_str(), TextAlign );
	cfg->getValue( "font", id.c_str(), font );
	cfg->getValue( "fontsize", id.c_str(), fontsize );
	cfg->getValue( "fontcolor", id.c_str(), vcolor );
	cfg->getValue( "lineheight", id.c_str(), lineheight );
	if( !lineheight )
		lineheight = 1.0;

	delete cfg;

	TextSprite.setPosition( TextPos.x, TextPos.x, getZ() );
	TextSprite.setFont( font, fontsize );
	TextSprite.setFixed( true );
	TextSprite.setLineHeight( lineheight );
	setWidgetText( "" );

	setFontColor( vcolor );

	if( !Widget::load( id ) )
		return false;

	return true;
}

void WidgetText::redraw( )
{
	if( !visible )
		return;

	float posx, posy;
	const rect2f& rect = getRect();

	s2f size( TextSprite.width(), TextSprite.height() );
	s2f dimension( rect.width, rect.height );
	if( size.x + TextPos.x > rect.width )
		dimension.x = size.x + TextPos.x;
	if( size.y + TextPos.y > rect.height )
		dimension.y = size.y + TextPos.y;
	this->resize( dimension.x, dimension.y );
	Widget::redraw();

	posx = getWidgetRealX() + TextPos.x;
	posy = getWidgetRealY() + TextPos.y;
	switch( TextAlign & (waLEFT | waRIGHT) ){
		case waMIDDLE:
			posx += 0.5f * ( rect.width - size.x );
			break;
		case waRIGHT:
			posx += rect.width - size.x;
			break;
		case waLEFT:
		default:
			break;
	}
	switch( TextAlign & (waTOP | waBOTTOM) ){
			case waMIDDLE:
				posy += 0.5f * ( rect.height - size.y );
				break;
			case waBOTTOM:
				break;
			case waTOP:
			default:
				posy += rect.height - TextSprite.getLineSize();
				//TextSprite.getLineSize() + TextPos.y;
				break;
		}
	TextSprite.setPosition( posx, posy, getZ( ) + 0.1f );
}

void WidgetText::setFontColor( const s4ub& clr )
{
	TextSprite.setColor( clr.r, clr.g, clr.b );
}

void WidgetText::setWidgetText( std::string text )
{
	if( TextContent == text )
		return;
	TextContent = text;
	TextSprite.setText( (BaseText + text).c_str() );
	redraw();
}

void WidgetText::setTextPosition( float x, float y )
{
	TextPos.x = x;
	TextPos.y = y;
	redraw();
}

void WidgetText::Update( )
{
	if( Binded.ptr == NULL )
		return;
	char d[6];
	const char* format;
	switch( Binded.type ){
		case tiInt:
			if( *(int*)Binded.ptr == BindedCache )
				return;
			BindedCache = *(int*)Binded.ptr;
			format = "%.0f";
			break;
		case tiFloat:
			if( *(float*)Binded.ptr == BindedCache )
				return;
			BindedCache = *(float*)Binded.ptr;
			format = "%.0f";
			break;
		default:
			Debug::debug( Debug::INTERFACE, "Binded variable type not supported.\n");
			return;
	}
	snprintf(d, 6, format, BindedCache);
	setWidgetText( d );
}

void WidgetText::toggleVisibility( )
{
	Widget::toggleVisibility( );
	TextSprite.setVisible(visible);
	// You need to show cursor every time. No remembering of old state.
	if( isCursorVisible() )
		clearCursorVisible();
}

