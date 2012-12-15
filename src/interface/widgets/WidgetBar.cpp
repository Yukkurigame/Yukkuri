/*
 * BarWidget.cpp
 *
 *  Created on: 24.03.2012
 *
 */

#include "interface/widgets/WidgetBar.h"
#include "graphics/Render.h"
#include "scripts/LuaConfig.h"

#include "safestring.h"


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
	RenderManager::FreeGLSprite( BarSprite );
	RenderManager::FreeGLSprite( TopSprite );
}

bool WidgetBar::load( std::string id )
{
	if( !WidgetText::load( id ) )
		return false;

	std::string imgname;
	int picture;
	int barheight;
	s4ub color;
	std::vector<unsigned int> vcolor;
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
	cfg->getValue( "barcolor", id, color );

	if( BarWidth <= 0 )
		BarWidth = (float)Width;
	createBar( imgname, picture, barheight, color );
	updatePosition();

	delete cfg;

	return true;
}

void WidgetBar::createBar( std::string name, int picture, int height, s4ub color )
{
	Height -= height + (int)BarY;
	BarSprite = RenderManager::CreateGLSprite( PosX + BarX, PosY + BarY, getZ(),(int)BarWidth, height );
	BarSprite->setFixed();
	if( name != "" ){
		TopSprite = RenderManager::CreateGLSprite( PosX + TopX, PosY + TopY, getZ() + 0.1f,
						(int)Width, (int)Height, RenderManager::GetTextureNumberById(name), picture );
		TopSprite->setFixed();
	}
	if( BarSprite )
		BarSprite->brush.set_color( color );
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
		setWidgetText( str );
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
		setWidgetText( str );
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
	if( Binded.type == tiFloat && Binded.ptr ){
		if( *(float*)Binded.ptr != BarValue )
			setBarValue( *(float*)Binded.ptr );
	}
	if( BindedMaxValue != NULL ){
		if( (*BindedMaxValue) != BarMaxValue )
			setBarSize( *BindedMaxValue );
	}
}

void WidgetBar::toggleVisibility( )
{
	WidgetText::toggleVisibility( );
	// FIXME: 1>yukkuri\interface\widgets\widgetbar.cpp(157): warning C4800: 'unsigned char' :
	//          forcing value to bool 'true' or 'false' (performance warning)
	if( BarSprite && (bool)BarSprite->isVisible() != visible )
		BarSprite->toggleVisibility( );
	if( TopSprite && (bool)TopSprite->isVisible() != visible )
		TopSprite->toggleVisibility( );
}


