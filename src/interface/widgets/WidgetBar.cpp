/*
 * BarWidget.cpp
 *
 *  Created on: 24.03.2012
 *
 */

#include "interface/widgets/WidgetBar.h"
#include "graphics/Render.h"
#include "graphics/render/Textures.h"
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




void WidgetBar::createBar( const char* name, int picture, const s4ub& color )
{
	BarSprite = RenderManager::CreateGLSprite( Bar.x, Bar.y, 1.0, Bar.width, Bar.height, -1 );
	if( BarSprite ){
			BarSprite->setFixed();
			BarSprite->clearLight();
			BarSprite->brush.set_color( color );
	}
	if( name && name != '\0' ){
		TopSprite = RenderManager::CreateGLSprite( Top.x,  Top.y, 1.1, Top.width, Top.height, -1,
						Textures::get_by_name( name ), picture );
		TopSprite->setFixed();
		TopSprite->clearLight();
	}
	setBarValue(1);
	setBarSize(1);
}

void WidgetBar::setBarValue( float value )
{
	if( value == BarValue )
		return;
	if( value < 0 )
		value = 0;
	if( value > BarMaxValue )
		value = BarMaxValue;
	BarValue = value;
	{ //Output text;
		char str[25];
		snprintf( str, 25, "%.0f/%.0f", value, BarMaxValue );
		setWidgetText( str );
	}
	redraw();
}

void WidgetBar::setBarSize( float val )
{
	if( val > 0 )
		BarMaxValue = val;
	else
		BarMaxValue = 1.0;
	{ //Output text;
		char str[25];
		snprintf( str, 25, "%.0f/%.0f", BarValue, BarMaxValue );
		setWidgetText( str );
	}
	redraw();
}



void WidgetBar::redraw( )
{
	if( !visible )
		return;

	const rect2f& rect = getRect();

	LOWER_LIMIT( Bar.width, 1.0, rect.width )
	LOWER_LIMIT( Bar.height, 1.0, rect.height )

	float rate = BarValue / BarMaxValue;
	s2f size = Bar.size() * s2f( rate, 1.0f );
	s2f dimension;
	dimension.x = MAX( size.x, rect.width );
	dimension.y = MAX( size.y, rect.height );
	this->resize( dimension.x, dimension.y );
	WidgetText::redraw();

	s3f pos = getWidgetPosition();
	if( BarSprite ){
		BarSprite->resize( size.x, size.y, 0 );
		BarSprite->setPosition( pos.x + Bar.x, pos.y + Bar.y, pos.z );
	}
	if( TopSprite ){
		LOWER_LIMIT( Top.width, 1.0, rect.width )
		LOWER_LIMIT( Top.height, 1.0, rect.height )
		TopSprite->resize( Top.width, Top.height, 0 );
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


