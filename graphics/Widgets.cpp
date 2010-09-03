/*
 * Widgets.cpp
 *
 *  Created on: 30.06.2010
 */
#include "Widgets.h"
#include "config.h"

Widget::Widget()
{
	graph = Graphics::Instance();
	type = NONE;
	width = 0;
	height = 0;
	posx = 0;
	posy = 0;
	posz = 0;
	visible = true;
	parent = NULL;
	background = NULL;
}

Widget::~Widget()
{
	graph->FreeGLSprite( background );
	parent = NULL;
	children.clear();
}

bool Widget::create( string name, string text, int x, int y )
{
	//FIXME: text not used it is bad;
	Texture* tex;
	tex = graph->LoadGLTexture( name );
	background = graph->CreateGLSprite(posx, posy, getZ(), x, y, width, height, tex);
	if(!background)
		return false;
	return true;
}

float Widget::getZ( )
{
	extern MainConfig conf;
	return posz + conf.widgetsPosZ;
}

void Widget::setParent( Widget* p )
{
	parent = p;
	posx += p->posx;
	posy = p->posy + p->height - posy;
	posz = posz + p->posz + 0.1;
	if( background )
		background->vertices->z = getZ();
}

void Widget::addChild( Widget* child )
{
	children.push_back( child );
}

void Widget::toggleVisibility( )
{
	if( this->visible )
		visible = false;
	else
		visible = true;
	if( this->background && this->background->visible != visible )
		this->background->toggleVisibility( );
	for( int i = 0, end = children.size(); i < end; ++i ){
		children[i]->toggleVisibility();
	}
}

TextWidget::TextWidget( )
{
	FontName = "DejaVuSans";
	FontSize = 12;
	textx = 0;
	texty = 0;
	Text = "";
	StaticTextSprite = NULL;
	TextSprite = NULL;
}

TextWidget::~TextWidget( )
{
	graph->FreeTextSprite( TextSprite );
	graph->FreeTextSprite( StaticTextSprite );
}

bool TextWidget::create( string name, string text, int x, int y )
{
	if( name == "" ){
		background = NULL;
	}else{
		if( !Widget::create( name, text, x, y ) )
			background = NULL;
	}
	if( text != "" ){
		StaticTextSprite = graph->CreateGLSprite( x, y, getZ(), 20, 20, width, height, NULL );
		StaticTextSprite->clr->set( 0 );
	}
	Text = text;
	TextSprite = graph->CreateGLSprite( x, y, getZ(), 20, 20, width, height, NULL );
	TextSprite->clr->set( 0 );
	return true;
}

void TextWidget::setParent( Widget* p )
{
	float x = p->posx + posx + textx;
	float y = p->posy + p->height - posy - texty;
	Widget::setParent( p );
	textPosition( x, y );
}

void TextWidget::setFontColor( int r, int g, int b )
{
	if( StaticTextSprite )
		StaticTextSprite->clr->set( r, g, b );
	TextSprite->clr->set( r, g, b );
}

void TextWidget::setText( string text )
{
	if( AddText == text )
		return;
	if( StaticTextSprite ){
		if( Text != "" && !StaticTextSprite->tex ){
			graph->ChangeTextSprite( StaticTextSprite, FontName, FontSize, Text, 1 );
			TextSprite->setPosition( TextSprite->posx + StaticTextSprite->width, TextSprite->posy );
		}
	}
	AddText = text;
	graph->ChangeTextSprite( TextSprite, FontName, FontSize, AddText );
}

void TextWidget::setTextPosition( float x, float y )
{
	textx = x;
	texty = y;
	textPosition( posx + x, posy + height - y );
}

void TextWidget::toggleVisibility( )
{
	Widget::toggleVisibility( );
	if( StaticTextSprite && StaticTextSprite->visible != visible )
		StaticTextSprite->toggleVisibility( );
	if( TextSprite && TextSprite->visible != visible )
		TextSprite->toggleVisibility( );
}

void TextWidget::textPosition( float x, float y )
{
	float txdelta;
	txdelta = 0;
	if( StaticTextSprite ){
		StaticTextSprite->setPosition( x, y, getZ( ) + 0.1 );
		txdelta = StaticTextSprite->width;
	}
	TextSprite->setPosition( x + txdelta, y, getZ( ) + 0.1 );
}

BarWidget::BarWidget()
{
	bar = NULL;
	top = NULL;
	barwidth = 0;
	barmaxvalue = 1;
	barstartx = 0;
}

BarWidget::~BarWidget( )
{
	graph->FreeGLSprite( top );
	graph->FreeGLSprite( bar );
}

void BarWidget::createBar( string name, int* pos)
{
	Texture* tex;
	barstartx = posx + pos[2];
	if( pos[4] > 0 )
		barwidth = pos[4];
	else
		barwidth = width;
	tex = graph->LoadGLTexture( name );
	bar = graph->CreateGLSprite( barstartx, posy + pos[3], getZ(), barwidth, pos[5] );
	if( tex )
		top = graph->CreateGLSprite( posx, posy, getZ() + 0.1, pos[0], pos[1], width, height, tex );
	if( bar ){
		bar->clr->set( pos[6], pos[7], pos[8] );
	}
	setTextPosition( getTextX() + 5, getTextY() - pos[5] );
	setBarValue(1);
	setBarSize(1);
}

void BarWidget::setBarValue( int value )
{
	if( value == barvalue )
		return;
	barvalue = value;
	{//Output text;
		char str[25];
		sprintf( str, "%d/%d", value, barmaxvalue );
		setText( str );
	}
	if( value < 0 )
		value = 0;
	if( value > barmaxvalue )
		value = barmaxvalue;
	float s = static_cast<float>(value) / static_cast<float>(barmaxvalue);
	float x = barwidth * s;
	bar->resize( x, -1 );
}

void BarWidget::setParent( Widget* p )
{
	float bardelta = barstartx - posx;
	float x = p->posx + posx;
	float y = p->posy + p->height - posy;
	float barheight = bar->vertices->lt.y - bar->vertices->lb.y;
	TextWidget::setParent( p );
	bar->setPosition( x + bardelta, y, getZ() );
	if( top )
		top->setPosition( x, y - barheight, getZ() + 0.01 );
}

void BarWidget::toggleVisibility( )
{
	TextWidget::toggleVisibility( );
	if( bar && bar->visible != visible )
		bar->toggleVisibility( );
	if( top && top->visible != visible )
			top->toggleVisibility( );
}
