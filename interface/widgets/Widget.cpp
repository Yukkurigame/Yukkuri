/*
 * Widgets.cpp
 *
 *  Created on: 30.06.2010
 */

#include "widgets/Widget.h"

#include "Render.h"
#include "scripts/LuaConfig.h"
#include "config.h"

#include "debug.h"
#include "hacks.h"


Widget::Widget()
{
	Type = wtNone;
	Width = 0;
	Height = 0;
	PosX = 0;
	PosY = 0;
	PosZ = 0;
	Align = wtNone;
	visible = true;
	Parent = NULL;
	background = NULL;
	iBinded = NULL;
	fBinded = NULL;
}

Widget::~Widget()
{
	RenderManager::Instance()->FreeGLSprite( background );
	Parent = NULL;
	//FIXME: parent dies but children overcomes this.
	Children.clear();
}

bool Widget::load( std::string id )
{
	LuaConfig* cfg = new LuaConfig;
	std::string align;
	std::string valign;

	baseID = id;

	if( ! cfg->getValue( "name", baseID, Name ))
		return false;
	cfg->getValue( "x", baseID, OffsetX );
	cfg->getValue( "y", baseID, OffsetY );
	cfg->getValue( "width", baseID, Width );
	cfg->getValue( "height", baseID, Height );
	cfg->getValue( "align", baseID, align );
	cfg->getValue( "valign", baseID, valign );

	if( align == "Center" )
		Align = CENTER;
	else if( align == "Right" )
		Align = RIGHT;
	else
		Align = LEFT;

	if( valign == "Middle" )
		VAlign = MIDDLE;
	else if( valign == "Bottom" )
		VAlign = BOTTOM;
	else
		VAlign = TOP;

	int z = 0;
	cfg->getValue("depth", baseID, z );
	setZ( z );

	updatePosition( );
	if( Type != wtNone ){
		std::string imgname;
		int picture = 0;
		std::vector< int > bgcolor;
		cfg->getValue( "image", baseID, imgname );
		cfg->getValue( "picture", baseID, picture );
		cfg->getValue( "bgcolor", baseID, bgcolor );
		if( imgname != "" || bgcolor.size() ){
			background = RenderManager::Instance()->CreateGLSprite( PosX, PosY, getZ(), Width, Height,
					RenderManager::Instance()->GetTextureNumberById( imgname ), picture );
			background->setFixed();
			if( bgcolor.size() ){
				for( unsigned int i=0; i < 4; ++i ){
					if( i >= bgcolor.size() )
						bgcolor.push_back( i < 3 ? 0 : 255 );
				}
				background->clr.set( bgcolor[0], bgcolor[1], bgcolor[2], bgcolor[3] );
			}
		}
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
			posy = starty - height * 0.5f - this->Height * 0.5f - OffsetY;
			break;
		case BOTTOM:
			posy = starty - height - OffsetY;
			break;
		case TOP:
		default:
			posy = starty - this->Height - OffsetY;
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


Widget* Widget::getChildren( std::string name )
{
	FOREACHIT( Children ){
		if( (*it)->getName() == name )
			return *it;
	}
	return NULL;
}

bool Widget::bindValue( int* val )
{
	if( val ){
		iBinded = val;
		return true;
	}
	iBinded = NULL;
	return false;
}

bool Widget::bindValue( float* val )
{
	if( val ){
		fBinded = val;
		return true;
	}
	fBinded = NULL;
	return false;
}

void Widget::toggleVisibility( )
{
	if( this->visible )
		visible = false;
	else
		visible = true;
	if( this->background && this->background->isVisible() != visible )
		this->background->toggleVisibility( );
	for( int i = 0, end = Children.size(); i < end; ++i ){
		Children[i]->toggleVisibility();
	}
}

