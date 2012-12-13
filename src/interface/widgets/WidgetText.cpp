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
	TextX = 0;
	TextY = 0;
	TextAlign = waCENTER;
	TextVAlign = waMIDDLE;
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
	std::vector<int> vcolor;
	LuaConfig* cfg = new LuaConfig;

	cfg->getValue( "text", id, BaseText );
	cfg->getValue( "textx", id, TextX );
	cfg->getValue( "texty", id, TextY );
	cfg->getValue( "textalign", id, talign );
	cfg->getValue( "textvalign", id, tvalign );
	cfg->getValue( "font", id, font );
	cfg->getValue( "fontsize", id, fontsize );
	cfg->getValue( "fontcolor", id, vcolor );
	cfg->getValue( "lineheight", id, lineheight );
	if( !lineheight )
		lineheight = 1.0;

	delete cfg;

	if( talign == "Center" )
		TextAlign = waCENTER;
	else if( talign == "Right" )
		TextAlign = waRIGHT;
	else
		TextAlign = waLEFT;
	if( tvalign == "Middle" )
		TextVAlign = waMIDDLE;
	else if( tvalign == "Bottom" )
		TextVAlign = waBOTTOM;
	else
		TextVAlign = waTOP;

	TextSprite.setPosition( TextX, TextX, getZ() );
	TextSprite.setFont( font, fontsize );
	TextSprite.setFixed( true );
	TextSprite.setLineHeight( lineheight );
	setWidgetText( "" );

	for( unsigned int i=0; i < 3; ++i ){
		if( i >= vcolor.size() )
			vcolor.push_back(0);
	}
	setFontColor( (unsigned)vcolor[0], (unsigned)vcolor[1], (unsigned)vcolor[2] );

	if( !Widget::load( id ) )
		return false;

	return true;
}

void WidgetText::updatePosition( )
{
	float posx, posy, height, width;
	width = (float)TextSprite.width();
	height = (float)TextSprite.height();
	if( width + TextX > Width )
		this->Width = width + TextX;
	if( height + TextY > Height )
		this->Height = height + TextY;
	Widget::updatePosition( );
	posx = posy = 0;
	switch(TextAlign){
		case waCENTER:
			posx = PosX + this->Width * 0.5f - width * 0.5f + TextX;
			break;
		case waRIGHT:
			posx = PosX + this->Width - width + TextX;
			break;
		case waLEFT:
		default:
			posx = PosX + TextX;
			break;
	}
	switch(TextVAlign){
			case waCENTER:
				posy = PosY - height * 0.5f + this->Height * 0.5f + TextY;
				break;
			case waBOTTOM:
				posy = PosY + TextY;
				break;
			case waTOP:
			default:
				posy = PosY + this->Height -
				TextSprite.getLineSize() + TextY;
				break;
		}
	TextSprite.setPosition( posx, posy, getZ( ) + 0.1f );
}

void WidgetText::setFontColor( unsigned int r, unsigned int g, unsigned int b )
{
	TextSprite.setColor( r, g, b );
}

void WidgetText::setWidgetText( std::string text )
{
	float w, h;
	if( TextContent == text )
		return;
	TextContent = text;
	TextSprite.setText( (BaseText + text).c_str() );
	w = (float)Width;
	h = (float)Height;
	if( !Width || Width < TextSprite.width() )
		w = (float)TextSprite.width();
	if( !Height || Height < TextSprite.height() )
		h = (float)TextSprite.height();
	resize( w, h );
	updatePosition();
}

void WidgetText::setTextPosition( float x, float y )
{
	TextX = x;
	TextY = y;
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

