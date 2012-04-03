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
	//RenderManager::Instance()->FreeTextSprite( TextSprite );
}


bool TextWidget::load( std::string config )
{
	if( !Widget::load( config ) )
		return false;

	std::string font;
	int fontsize = 12;
	std::vector<int> vcolor;
	LuaConfig* cfg = new LuaConfig;
	//float textx, texty;
	//textx = texty = 0;

	cfg->getValue( "textx", config, "widget", TextX );
	cfg->getValue( "texty", config, "widget", TextY );
	cfg->getValue( "textalign", config, "widget", TextAlign );
	cfg->getValue( "font", config, "widget", font );
	cfg->getValue( "fontsize", config, "widget", fontsize );
	cfg->getValue( "fontcolor", config, "widget", vcolor );

	TextSprite.setPosition( TextX, TextX, getZ() );
	TextSprite.setFont( font, fontsize );
	//TextSprite.setFixed( true );
	TextSprite.setText( "The quick brown fox jumps over the \n lazy dog." );

	//RenderManager::Instance()->CreateGLSprite( TextX, TextX, getZ(), Width, Height, (TextureInfo*)NULL, 0 );

	updatePosition( );

	setFont( font, fontsize );
	if( vcolor.size( ) > 2 )
		setFontColor(vcolor[0], vcolor[1], vcolor[2]);

	delete cfg;

	return true;
}

void TextWidget::updatePosition( )
{
	float posx, posy, swidth, width, height;
	Widget::updatePosition( );
	posx = posy = swidth = width = height = 0;
	//if( TextSprite ){
		//width = TextSprite->width;
		//if( !height || height < TextSprite->height )
		//	height = TextSprite->height;
	//}
	switch(TextAlign){
		case CENTER:
			posx = PosX + this->Width * 0.5 - ( swidth + width ) * 0.5 + TextX;
			break;
		case RIGHT:
			posx = PosX + this->Width - ( swidth + width ) + TextX;
			break;
		case LEFT:
		default:
			posx = PosX + TextX;
			break;
	}
	posy = PosY - TextY;
	//if( TextSprite )
		TextSprite.setPosition( posx + swidth, posy, getZ( ) + 0.1f );
}

void TextWidget::setFontColor( int r, int g, int b )
{
	//TextSprite->clr.set( r, g, b );
}

void TextWidget::setText( std::string text )
{
	float w, h;
	if( TextContent == text )
		return;
	TextContent = text;
	TextSprite.setText( text.c_str() );
	//RenderManager::Instance()->ChangeTextSprite( TextSprite, FontName, FontSize, Text );
	w = Width;
	h = Height;
	//if( !Width || Width < TextSprite->width ){
	//	w = TextSprite->width;
	//}
	//if( !Height || Height < TextSprite->height ){
//		h = TextSprite->height;
	//}
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
	//if( TextSprite && TextSprite->visible != visible )
	//	TextSprite->toggleVisibility( );
}
