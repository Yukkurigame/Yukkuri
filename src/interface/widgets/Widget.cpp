/*
 * Widgets.cpp
 *
 *  Created on: 30.06.2010
 */

#include "interface/widgets/Widget.h"

#include "graphics/Render.h"
#include "scripts/LuaConfig.h"
#include "config.h"

#include "debug.h"
#include "hacks.h"

#include "3rdparty/timer/TimerManager.h"


void Widget::WCaller::start( )
{
	event_id = Timer::AddInternalTimerEvent( this, 1, UPDATE_PERIOD, 0, true, false );
}

void Widget::WCaller::stop( )
{
	if(event_id < 0)
		return;
	if( Timer::DeleteTimerEventById( event_id ) )
		event_id = -1;
	else
		Debug::debug( Debug::INTERFACE, "Cannot delete timer event.\n" );
}



Widget::Widget() : Timer(this)
{
	ID = 0;
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
	Binded.ptr = NULL;
	Binded.type = tiNone;
}


Widget::~Widget()
{
	RenderManager::FreeGLSprite( background );
	Parent = NULL;
	//FIXME: parent dies but children overcomes this.
}


bool Widget::create( std::string id )
{
	baseID = id;
	return true;
}


bool Widget::load( std::string id )
{
	LuaConfig* cfg = new LuaConfig;
	std::string align;
	std::string valign;

	if( !(	this->create( id ) &&
			cfg->getValue( "name", id, Name ) ) )
		return false;

	cfg->getValue( "x", baseID, OffsetX );
	cfg->getValue( "y", baseID, OffsetY );
	cfg->getValue( "width", baseID, Width );
	cfg->getValue( "height", baseID, Height );
	cfg->getValue( "align", baseID, align );
	cfg->getValue( "valign", baseID, valign );

	if( align == "Center" )
		Align = waCENTER;
	else if( align == "Right" )
		Align = waRIGHT;
	else
		Align = waLEFT;

	if( valign == "Middle" )
		VAlign = waMIDDLE;
	else if( valign == "Bottom" )
		VAlign = waBOTTOM;
	else
		VAlign = waTOP;


	float z = 0;
	cfg->getValue("depth", baseID, z );
	setWidgetRealZ( z );

	updatePosition( );

	{
		std::string imgname;
		int picture = 0;
		std::vector< int > bgcolor;
		cfg->getValue( "image", baseID, imgname );
		cfg->getValue( "picture", baseID, picture );
		cfg->getValue( "bgcolor", baseID, bgcolor );
		if( imgname != "" || bgcolor.size() ){
			setBackground( RenderManager::GetTextureNumberById( imgname ), picture );
			if( bgcolor.size() ){
				for( unsigned int i=0; i < 4; ++i ){
					if( i >= bgcolor.size() )
						bgcolor.push_back( i < 3 ? 0 : 255 );
				}
				setBackgroundColor( (unsigned)bgcolor[0], (unsigned)bgcolor[1],
									(unsigned)bgcolor[2], (unsigned)bgcolor[3] );
			}
		}
	}

	delete cfg;

	return true;
}


void Widget::setBackground( int texture, int picture )
{
	if( Type == wtNone ) // No background for null widget
		return;
	if( background )
		RenderManager::FreeGLSprite( background );
	background = RenderManager::CreateGLSprite( PosX, PosY, getZ(), (int)Width, (int)Height,
							texture, picture );
	background->setFixed();
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
		background->resize( (float)Width, (float)Height );
}


void Widget::updatePosition( )
{
	extern MainConfig conf;
	float posx, posy, startx, starty, width, height;
	if(Parent){
		width = Parent->getWidgetWidth( );
		height = Parent->getWidgetHeight( );
		startx = Parent->getWidgetRealX( );
		starty = Parent->getWidgetRealY( ) + height;
	}else{
		startx = 0;
		starty = (float)conf.windowHeight;
		width = (float)conf.windowWidth;
		height = (float)conf.windowHeight;
	}
	switch(Align){
		case waCENTER:
			posx = startx + width * 0.5f - this->Width * 0.5f + OffsetX;
			break;
		case waRIGHT:
			posx = startx + width - this->Width + OffsetX;
			break;
		case waLEFT:
		default:
			posx = startx + OffsetX;
			break;
	}
	switch(VAlign){
		case waCENTER:
			posy = starty - height * 0.5f - this->Height * 0.5f - OffsetY;
			break;
		case waBOTTOM:
			posy = starty - height - OffsetY;
			break;
		case waTOP:
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


Widget* Widget::getChildren( std::string name )
{
	listElement<Widget*>* l = Children.head;
	while( l != NULL ){
		if( l->data->getWidgetName() == name )
			return l->data;
		l = l->next;
	}
	return NULL;
}


bool Widget::bindValue( enum type_identifier type, void* val )
{
	if( val != 0 ){
		Binded.ptr = val;
		Binded.type = type;
		Timer.start();
		return true;
	}
	Timer.stop();
	Binded.ptr = NULL;
	Binded.type = tiNone;
	return false;
}


void Widget::toggleVisibility( )
{
	if( this->visible )
		visible = false;
	else
		visible = true;
	// FIXME: 1>yukkuri\interface\widgets\widget.cpp(218): warning C4800: 'unsigned char' :
	//          forcing value to bool 'true' or 'false' (performance warning)
	if( this->background && (bool)this->background->isVisible() != visible )
		this->background->toggleVisibility( );

	listElement<Widget*>* l = Children.head;
	while( l != NULL ){
		l->data->toggleVisibility();
		l = l->next;
	}
}

