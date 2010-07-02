/*
 * Widgets.cpp
 *
 *  Created on: 30.06.2010
 */
#include "Widgets.h"

Widget::Widget()
{
	width = 0;
	height = 0;
	posx = 0;
	posy = 0;
	visible = true;
	parent = NULL;
	background = NULL;
}

Widget::~Widget()
{
	Graphics::graph.FreeGLSprite( background );
	parent = NULL;
	children.clear();
}

bool Widget::create( string name )
{
	Texture* tex;
	tex = Graphics::graph.LoadGLTexture( name );
	//FIXME: как-то много
	background = Graphics::graph.CreateGLSprite( posx, posy, 0, width, height, tex);
	if(!background)
		return false;
	return true;
}

void Widget::addChild( Widget* child )
{
	children.push_back( child );
}

void Widget::draw( )
{
	if(!this->visible)
		return;
	if(!this->background)
		return;
	Graphics::graph.DrawGLTexture( background );
}

TextWidget::TextWidget()
{
	FontName = "DejaVuSans";
	FontSize = 12;
	textx = 0;
	texty = 0;
	Text = "";
}

bool TextWidget::create( string name )
{
	if( name == "" ){
		background = NULL;
	}else{
		Texture* tex;
		tex = Graphics::graph.LoadGLTexture( name );
		background = Graphics::graph.CreateGLSprite( posx, posy, 0, width, height, tex);
	}
	return true;
}

void TextWidget::draw( )
{
	if(!this->visible)
		return;
	if(background)
		Graphics::graph.DrawGLTexture( background );
	Graphics::graph.PrintText( FontName, FontSize, posx+textx, posy+texty, Text );
}
