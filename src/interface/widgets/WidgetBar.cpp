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
	BarMaxValue = 1;
	BarValue = 0;
	BindedMaxValue = NULL;
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
	s4ub color;
	std::vector<unsigned int> vcolor;
	LuaConfig* cfg = new LuaConfig;

	//Order: topimgx, topimgy, barheight, r, g, b
	//Ya, it's cruve, but it's simple
	cfg->getValue( "barheight", id, Bar.height );
	cfg->getValue( "barwidth", id, Bar.width );
	cfg->getValue( "barx", id, Bar.x );
	cfg->getValue( "bary", id, Bar.y );
	cfg->getValue( "topimage", id, imgname );
	cfg->getValue( "toppicture", id, picture );
	cfg->getValue( "barcoverx", id, Top.x );
	cfg->getValue( "barcovery", id, Top.y );
	cfg->getValue( "barcolor", id, color );

	if( Bar.width <= 0 )
		Bar.width = (float)Rect.width;
	if( Bar.height <= 0 )
		Bar.height = (float)Rect.height;
	createBar( imgname, picture, color );
	updatePosition();

	delete cfg;

	return true;
}


void WidgetBar::createBar( std::string name, int picture, s4ub color )
{
	BarSprite = RenderManager::CreateGLSprite( Bar.x, Bar.y, 1.0, Bar.width, Bar.height );
	if( BarSprite ){
			BarSprite->setFixed();
			BarSprite->brush.set_color( color );
	}
	if( name != "" ){
		TopSprite = RenderManager::CreateGLSprite( Top.x,  Top.y, 1.1, Top.width, Top.height,
						RenderManager::GetTextureNumberById(name), picture );
		TopSprite->setFixed();
	}
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
		BarSprite->resize( Bar.width * value / BarMaxValue, Bar.height );
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
		BarSprite->resize( Bar.width * BarValue / BarMaxValue, -1 );
}

void WidgetBar::updatePosition( )
{
	WidgetText::updatePosition( );
	s3f pos = getWidgetPosition();
	if( BarSprite )
		BarSprite->setPosition( pos.x + Bar.x, pos.y + Bar.y, pos.z );
	if( TopSprite ){
		TopSprite->resize( Top.width, Top.height );
		TopSprite->setPosition( pos.x + Bar.x + Top.x, pos.y + Bar.y + Top.y, pos.z + 0.01f );
	}
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


