/*
 * BarWidget.cpp
 *
 *  Created on: 24.03.2012
 *
 */

#include "widgets/WidgetBar.h"
#include "Render.h"
#include "LuaConfig.h"


WidgetBar::WidgetBar()
{
	BarSprite = NULL;
	TopSprite = NULL;
	BarWidth = 0;
	BarMaxValue = 1;
	BindedMaxValue = NULL;
	BarX = 0;
	BarY = 0;
	BarValue = 0;
}

WidgetBar::~WidgetBar( )
{
	RenderManager::Instance()->FreeGLSprite( BarSprite );
	RenderManager::Instance()->FreeGLSprite( TopSprite );
}

bool WidgetBar::load( std::string id )
{
	if( !WidgetText::load( id ) )
		return false;

	std::string imgname;
	int picture;
	int barheight;
	color4u color;
	std::vector<int> vcolor;
	LuaConfig* cfg = new LuaConfig;

	//Order: topimgx, topimgy, barheight, r, g, b
	//Ya, it's cruve, but it's simple
	cfg->getValue( "barheight", id, barheight );
	cfg->getValue( "barwidth", id, BarWidth );
	cfg->getValue( "barx", id, BarX );
	cfg->getValue( "bary", id, BarY );
	cfg->getValue( "topimage", id, imgname );
	cfg->getValue( "toppicture", id, picture );
	cfg->getValue( "barcoverx", id, TopX );
	cfg->getValue( "barcovery", id, TopY );
	cfg->getValue( "barcolor", id, vcolor );

	for( unsigned int i=0; i < 3; ++i ){
		if( i >= vcolor.size() )
			vcolor.push_back(0);
	}
	color.set( vcolor[0], vcolor[1], vcolor[2] );

	if( BarWidth <= 0 )
		BarWidth = Width;
	createBar( imgname, picture, barheight, color );
	updatePosition();

	delete cfg;

	return true;
}

void WidgetBar::createBar( std::string name, int picture, int height, color4u clr )
{
	Height -= height + BarY;
	BarSprite = RenderManager::Instance()->CreateGLSprite( PosX + BarX, PosY + BarY, getZ(), BarWidth, height );
	BarSprite->fixed = true;
	if( name != "" ){
		TopSprite = RenderManager::Instance()->CreateGLSprite( PosX + TopX, PosY + TopY, getZ() + 0.1f,
						Width, Height, RenderManager::Instance()->GetTextureNumberById(name), picture );
		TopSprite->fixed = true;
	}
	if( BarSprite ){
		BarSprite->clr.set( clr.r, clr.g, clr.b, clr.a );
	}
	//setTextPosition( getTextX(), getTextY() - Height );
	setBarValue(1);
	setBarSize(1);
}

void WidgetBar::setBarValue( float value )
{
	if( value == BarValue )
		return;
	BarValue = value;
	{//Output text;
		char str[25];
		snprintf( str, 25, "%.0f/%.0f", value, BarMaxValue );
		setText( str );
	}
	if( value < 0 )
		value = 0;
	if( value > BarMaxValue )
		value = BarMaxValue;
	if( BarSprite )
		BarSprite->resize( BarWidth * value / BarMaxValue, -1 );
}

void WidgetBar::setBarSize( float val )
{
	if( val > 0 )
		BarMaxValue = val;
	else
		BarMaxValue = 1;
	{//Output text;
		char str[25];
		snprintf( str, 25, "%.0f/%.0f", BarValue, BarMaxValue );
		setText( str );
	}
	if( BarSprite )
		BarSprite->resize( BarWidth * BarValue / BarMaxValue, -1 );
}

void WidgetBar::updatePosition( )
{
	WidgetText::updatePosition( );
	if( BarSprite )
		BarSprite->setPosition( PosX + BarX, PosY + BarY, getZ() );
	if( TopSprite )
		TopSprite->setPosition( PosX + BarX + TopX, PosY + BarY + TopY, getZ() + 0.01f );
}

bool WidgetBar::bindBarMaxValue( float* val )
{
	if( val ){
		BindedMaxValue = val;
		return true;
	}
	return false;
}

void WidgetBar::Update( )
{
	if( Binded != NULL ){
		if( (*Binded) != BarValue )
			setBarValue( *Binded );
	}
	if( BindedMaxValue != NULL ){
		if( (*BindedMaxValue) != BarMaxValue )
			setBarSize( *BindedMaxValue );
	}
}

void WidgetBar::toggleVisibility( )
{
	WidgetText::toggleVisibility( );
	if( BarSprite && BarSprite->visible != visible )
		BarSprite->toggleVisibility( );
	if( TopSprite && TopSprite->visible != visible )
		TopSprite->toggleVisibility( );
}


