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

	cfg->getValue( "text", id, BaseText );
	cfg->getValue( "textx", id, TextPos.x );
	cfg->getValue( "texty", id, TextPos.y );
	cfg->getValue( "textalign", id, TextAlign );
	cfg->getValue( "font", id, font );
	cfg->getValue( "fontsize", id, fontsize );
	cfg->getValue( "fontcolor", id, vcolor );
	cfg->getValue( "lineheight", id, lineheight );
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

void WidgetText::updatePosition( )
{
	float posx, posy, height, width;
	width = (float)TextSprite.width();
	height = (float)TextSprite.height();
	if( width + TextPos.x > Rect.width )
		this->Rect.width = width + TextPos.x;
	if( height + TextPos.y > Rect.height )
		this->Rect.height = height + TextPos.y;
	Widget::updatePosition( );

	posx = getWidgetRealX() + TextPos.x;
	posy = getWidgetRealY() + TextPos.y;
	switch( TextAlign & (waLEFT | waRIGHT) ){
		case waMIDDLE:
			posx += 0.5f * (this->Rect.width - width);
			//posx = Rect.x + this->Rect.width * 0.5f - width * 0.5f + TextPos.x;
			break;
		case waRIGHT:
			posx += this->Rect.width - width;
			//posx = Rect.x + this->Rect.width - width + TextPos.x;
			break;
		case waLEFT:
		default:
			//posx = Rect.x + TextPos.x;
			break;
	}
	switch( TextAlign & (waTOP | waBOTTOM) ){
			case waMIDDLE:
				posy += 0.5f * ( this->Rect.height - height );
				//posy = Rect.y - height * 0.5f + this->Rect.height * 0.5f + TextPos.y;
				break;
			case waBOTTOM:
				//posy = Rect.y + TextPos.y;
				break;
			case waTOP:
			default:
				posy += this->Rect.height - TextSprite.getLineSize();
				//posy = Rect.y + this->Rect.height -
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
	float w, h;
	if( TextContent == text )
		return;
	TextContent = text;
	TextSprite.setText( (BaseText + text).c_str() );
	w = (float)Rect.width;
	h = (float)Rect.height;
	if( !Rect.width || Rect.width < TextSprite.width() )
		w = (float)TextSprite.width();
	if( !Rect.height || Rect.height < TextSprite.height() )
		h = (float)TextSprite.height();
	resize( w, h );
	updatePosition();
}

void WidgetText::setTextPosition( float x, float y )
{
	TextPos.x = x;
	TextPos.y = y;
	updatePosition();
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

