/*
 * BarWidget.cpp
 *
 *  Created on: 24.03.2012
 *
 */

#include "widgets/BarWidget.h"
#include "Render.h"
#include "Luaconfig.h"


BarWidget::BarWidget()
{
	BarSprite = NULL;
	TopSprite = NULL;
	BarWidth = 0;
	BarMaxValue = 1;
	BindedMaxValue = NULL;
	BarX = 0;
	BarY = 0;
}

BarWidget::~BarWidget( )
{
	RenderManager::Instance()->FreeGLSprite( BarSprite );
	RenderManager::Instance()->FreeGLSprite( TopSprite );
}

bool BarWidget::load( std::string config )
{
	if( !TextWidget::load( config ) )
		return false;
	std::string imgname;
	int picture;
	int barheight;
	color4u color;
	std::vector<int> vcolor;
	LuaConfig* cfg = new LuaConfig;

	//Order: topimgx, topimgy, barheight, r, g, b
	//Ya, it's cruve, but it's simple
	cfg->getValue( "barheight", config, "widget", barheight );
	cfg->getValue( "barwidth", config, "widget", BarWidth );
	cfg->getValue( "barx", config, "widget", BarX );
	cfg->getValue( "bary", config, "widget", BarY );
	cfg->getValue( "topimage", config, "widget", imgname );
	cfg->getValue( "toppicture", config, "widget", picture );
	cfg->getValue( "barcolor", config, "widget", vcolor );
	if( vcolor.size( ) > 2 ){
		color.set( vcolor[0], vcolor[1], vcolor[2] );
	}else{
		color.set(0);
	}
	if( BarWidth <= 0 )
		BarWidth = Width;
	createBar( imgname, picture, barheight, color );
	updatePosition();

	delete cfg;

	return true;
}

void BarWidget::createBar( std::string name, int picture, int height, color4u clr )
{
	BarSprite = RenderManager::Instance()->CreateGLSprite( PosX + BarX, PosY + BarY, getZ(), BarWidth, height );
	if( name != "" )
		TopSprite = RenderManager::Instance()->CreateGLSprite( PosX, PosY, getZ() + 0.1f,
						Width, Height, RenderManager::Instance()->GetTextureNumberById(name), picture );
	if( BarSprite ){
		BarSprite->clr.set( clr.r, clr.g, clr.b, clr.a );
	}
	setTextPosition( getTextX(), getTextY() - Height );
	setBarValue(1);
	setBarSize(1);
}

void BarWidget::setBarValue( float value )
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

void BarWidget::setBarSize( float val )
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

void BarWidget::updatePosition( )
{
	TextWidget::updatePosition( );
	if( BarSprite )
		BarSprite->setPosition( PosX + BarX, PosY + BarY, getZ() );
	if( TopSprite )
		TopSprite->setPosition( PosX, PosY, getZ() + 0.01f );
}

bool BarWidget::bindBarMaxValue( float* val )
{
	if( val ){
		BindedMaxValue = val;
		return true;
	}
	return false;
}

void BarWidget::Update( )
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

void BarWidget::toggleVisibility( )
{
	TextWidget::toggleVisibility( );
	if( BarSprite && BarSprite->visible != visible )
		BarSprite->toggleVisibility( );
	if( TopSprite && TopSprite->visible != visible )
		TopSprite->toggleVisibility( );
}


