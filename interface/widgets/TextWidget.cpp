/*
 * TextWidget.cpp
 *
 *  Created on: 24.03.2012
 *
 */

#include "widgets/TextWidget.h"
#include "Render.h"
#include "Luaconfig.h"


TextWidget::TextWidget( )
{
	FontName = "DejaVuSans";
	FontSize = 12;
	TextX = 0;
	TextY = 0;
	TextAlign = NONE;
	TextContent = "";
	BindedCache = 0.00123f;
}


TextWidget::~TextWidget( )
{

}


bool TextWidget::load( std::string id )
{
	std::string font;
	std::string text;
	std::string talign;
	int fontsize = 12;
	std::vector<int> vcolor;
	LuaConfig* cfg = new LuaConfig;

	cfg->getValue( "text", id, BaseText );
	cfg->getValue( "textx", id, TextX );
	cfg->getValue( "texty", id, TextY );
	cfg->getValue( "textalign", id, talign );
	cfg->getValue( "font", id, font );
	cfg->getValue( "fontsize", id, fontsize );
	cfg->getValue( "fontcolor", id, vcolor );

	delete cfg;

	if( talign == "Center" )
		TextAlign = CENTER;
	else if( talign == "Right" )
		TextAlign = RIGHT;
	else
		TextAlign = LEFT;

	TextSprite.setPosition( TextX, TextX, getZ() );
	TextSprite.setFont( font, fontsize );
	TextSprite.setFixed( true );
	setText( "" );

	if( vcolor.size( ) > 2 )
		setFontColor(vcolor[0], vcolor[1], vcolor[2]);

	if( !Widget::load( id ) )
		return false;

	return true;
}

void TextWidget::updatePosition( )
{
	float posx, posy, height, width;
	width = TextSprite.width();
	height = TextSprite.height();
	if( width + TextX > Width )
		this->Width = width + TextX;
	if( height + TextY > Height )
		this->Height = height + TextY;
	Widget::updatePosition( );
	posx = posy = 0;
	switch(TextAlign){
		case CENTER:
			posx = PosX + this->Width * 0.5 - width * 0.5 + TextX;
			break;
		case RIGHT:
			posx = PosX + this->Width - width + TextX;
			break;
		case LEFT:
		default:
			posx = PosX + TextX;
			break;
	}
	posy = PosY - TextY;
	TextSprite.setPosition( posx, posy, getZ( ) + 0.1f );
}

void TextWidget::setFontColor( int r, int g, int b )
{
	TextSprite.setColor( r, g, b );
}

void TextWidget::setText( std::string text )
{
	float w, h;
	if( TextContent == text )
		return;
	TextContent = text;
	TextSprite.setText( (BaseText + text).c_str() );
	w = Width;
	h = Height;
	if( !Width || Width < TextSprite.width() )
		w = TextSprite.width();
	if( !Height || Height < TextSprite.height() )
		h = TextSprite.height();
	resize( w, h );
	updatePosition();
}

void TextWidget::setTextPosition( float x, float y )
{
	TextX = x;
	TextY = y;
	updatePosition();
}

void TextWidget::Update( )
{
	if( !Binded )
		return;
	if( (*Binded) != BindedCache ){
		BindedCache = (*Binded);
		char d[6];
		snprintf(d, 6, "%.0f", BindedCache);
		setText( d );
	}
}

void TextWidget::toggleVisibility( )
{
	Widget::toggleVisibility( );
	TextSprite.setVisible(visible);
}
