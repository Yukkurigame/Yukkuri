/*
 * Widgets.cpp
 *
 *  Created on: 30.06.2010
 */

#include "widgets/Widget.h"

#include "Render.h"
#include "Luaconfig.h"
#include "config.h"

#include "debug.h"


Widget::Widget()
{
	Type = NONE;
	Width = 0;
	Height = 0;
	PosX = 0;
	PosY = 0;
	PosZ = 0;
	Align = NONE;
	visible = true;
	Parent = NULL;
	background = NULL;
	Binded = NULL;
}

Widget::~Widget()
{
	RenderManager::Instance()->FreeGLSprite( background );
	Parent = NULL;
	//FIXME: parent dies but children overcomes this.
	Children.clear();
}

bool Widget::load( std::string config )
{
	LuaConfig* cfg = new LuaConfig;
	cfg->getValue( "id", config, "widget", baseID );
	cfg->getValue( "x", config, "widget", OffsetX );
	cfg->getValue( "y", config, "widget", OffsetY );
	cfg->getValue( "width", config, "widget", Width );
	cfg->getValue( "height", config, "widget", Height );
	cfg->getValue( "align", config, "widget", Align );
	cfg->getValue( "valign", config, "widget", VAlign );
	updatePosition( );

	int z = 0;
	cfg->getValue("depth", config, "widget", z );
	setZ( z );

	if( Type != NONE ){
		std::string imgname;
		int picture;
		cfg->getValue( "image", config, "widget", imgname );
		cfg->getValue( "image", config, "widget", picture );
		background = RenderManager::Instance()->CreateGLSprite( PosX, PosY, getZ(), Width, Height,
				RenderManager::Instance()->GetTextureById( imgname ), picture );
		background->fixed = true;
	}

	delete cfg;

	return true;
}

void Widget::resize( float w, float h )
{
	if( w == Width && h == Height )
		return;
	if( w >= 0 )
		Width = w;
	if( h >= 0 )
		Height = h;
	if( background )
		background->resize( Width, Height );
}

void Widget::updatePosition( )
{
	extern MainConfig conf;
	float posx, posy, startx, starty, width, height;
	if(Parent){
		width = Parent->getWidth( );
		height = Parent->getHeight( );
		startx = Parent->getX( );
		starty = Parent->getY( ) + height;
	}else{
		startx = 0;
		starty = conf.windowHeight;
		width = conf.windowWidth;
		height = conf.windowHeight;
	}
	switch(Align){
		case CENTER:
			posx = startx + width * 0.5f - this->Width * 0.5f + OffsetX;
			break;
		case RIGHT:
			posx = startx + width - this->Width + OffsetX;
			break;
		case LEFT:
		default:
			posx = startx + OffsetX;
			break;
	}
	switch(VAlign){
		case CENTER:
			posy = starty - height * 0.5f + this->Height * 0.5f - OffsetY;
			break;
		case BOTTOM:
			posy = starty - height + this->Height - OffsetY;
			break;
		case TOP:
		default:
			posy = starty - OffsetY - this->Height;
			break;
	}
	PosX = posx;
	PosY = posy;
	if( background ){
		background->setPosition( PosX, PosY, getZ() );
	}

}

float Widget::getZ( )
{
	extern MainConfig conf;
	return PosZ + conf.widgetsPosZ;
}

void Widget::setParent( Widget* p )
{
	extern MainConfig conf;
	Parent = p;
	PosZ = PosZ + p->getZ( ) - conf.widgetsPosZ + 0.1f;
	updatePosition();
}

void Widget::addChild( Widget* child )
{
	Children.push_back( child );
}

/* This function needs memory allocation. */
void Widget::getChildren( Widget* children[], int size )
{
	for( int i = 0, vsz = Children.size(); i < vsz; ++i ){
		if( i >= size ) break;
		children[i] = Children[i];
	}
}

bool Widget::bindValue( float* val )
{
	if( val ){
		Binded = val;
		return true;
	}
	return false;
}

void Widget::toggleVisibility( )
{
	if( this->visible )
		visible = false;
	else
		visible = true;
	if( this->background && this->background->visible != visible )
		this->background->toggleVisibility( );
	for( int i = 0, end = Children.size(); i < end; ++i ){
		Children[i]->toggleVisibility();
	}
}



