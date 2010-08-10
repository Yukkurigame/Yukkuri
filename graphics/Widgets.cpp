/*
 * Widgets.cpp
 *
 *  Created on: 30.06.2010
 */
#include "Widgets.h"

Widget::Widget()
{
	graph = Graphics::Instance();
	width = 0;
	height = 0;
	posx = 0;
	posy = 0;
	posz = 1;
	visible = true;
	parent = NULL;
	background = NULL;
	bgimg[0] = 0;
	bgimg[1] = 0;
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
	bgimg[0] = x;
	bgimg[1] = y;
	background = graph->CreateGLSprite(posx, posy, posz, bgimg[0], bgimg[1], width, height, tex);
	if(!background)
		return false;
	return true;
}

void Widget::setParent( Widget* p )
{
	parent = p;
	posx += p->posx;
	posy = p->posy + p->height - posy;
	posz = posz - 0.5 + p->posz;
	if( background )
		background->vertices->z = posz;
}

void Widget::addChild( Widget* child )
{
	children.push_back( child );
}

void Widget::toggleVisibility( )
{
	if( this->visible ){
		visible = false;
	}else{
		visible = true;
	}
	for( int i = 0, end = children.size(); i < end; ++i ){
		children[i]->toggleVisibility();
	}
}

void Widget::draw( )
{
	if(!this->visible)
		return;
	if(!this->background)
		return;
	graph->DrawGLTexture( background );
}

TextWidget::TextWidget()
{
	FontName = "DejaVuSans";
	FontSize = 12;
	textx = 0;
	texty = 0;
	Text = "";
	TextSprite = NULL;
}

bool TextWidget::create( string name, string text, int x, int y )
{
	if( name == "" ){
		background = NULL;
	}else{
		if( !Widget::create( name, text, x, y ) )
			background = NULL;
	}
	Text = text;
	TextSprite = graph->CreateGLSprite( x, y, posz, 20, 20, width, height, NULL );
	TextSprite->clr->set( 0 );
	return true;
}

void TextWidget::setParent( Widget* p )
{
	float x = p->posx + posx + textx;
	float y = p->posy + p->height - posy - texty;
	Widget::setParent( p );
	TextSprite->setPosition( x, y );
	TextSprite->vertices->z = getZ();
}

void TextWidget::setFontColor( int r, int g, int b )
{
	TextSprite->clr->set( r, g, b );
}

void TextWidget::setText( string text )
{
	if( AddText == text )
		return;
	AddText = text;
	graph->ChangeTextSprite( TextSprite, FontName, FontSize, Text + AddText );
}

void TextWidget::setTextPosition( float x, float y )
{
	textx = posx + x;
	texty = posy + height - y;
	TextSprite->setPosition( textx, texty );
}

void TextWidget::draw( )
{
	if(!this->visible)
		return;
	if( this->FontName == "" )
		return;
	if(background)
		graph->DrawGLTexture( background );
	graph->DrawGLTexture( TextSprite );
	//graph->PrintText( FontName, FontSize, posx+textx, posy+texty, posz, FontColor, Text + AddText );
}

BarWidget::BarWidget()
{
	bar = NULL;
	top = NULL;
	barwidth = 0;
	barmaxvalue = 1;
	barstartx = 0;
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
	if( tex )
		top = graph->CreateGLSprite( posx, posy, posz + 0.1, pos[0], pos[1], width, height, tex );
	bar = graph->CreateGLSprite( barstartx, posy + pos[3], posz, barwidth, pos[5] );
	if( bar ){
		bar->clr->set( pos[6], pos[7], pos[8] );
	}
	setTextPosition( getTextX() - posx, getTextY() + pos[5] );
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
	bar->setPosition( x + bardelta, y );
	bar->vertices->z = getZ();
	if( top ){
		top->setPosition( x, y - barheight );
		top->vertices->z = getZ() + 0.01;
	}
}

void BarWidget::draw( )
{
	if(!this->visible)
			return;
	TextWidget::draw();
	graph->DrawGLTexture( bar );
	if( top )
		graph->DrawGLTexture( top );
}
