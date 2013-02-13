/*
 * Widgets.cpp
 *
 *  Created on: 30.06.2010
 */

#include "interface/widgets/Widget.h"

#include "graphics/Render.h"
#include "graphics/render/Textures.h"
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
	Align = waMIDDLE;
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


void Widget::setBackground( int texture, int picture )
{
	if( Type == wtNone ) // No background for null widget
		return;
	if( background )
		RenderManager::FreeGLSprite( background );
	background = RenderManager::CreateGLSprite( Position.x, Position.y, getZ(),
				(int)Rect.width, (int)Rect.height, texture, picture );
	background->setFixed();
	background->clearLight();
}


void Widget::resize( float w, float h )
{
	if( w == Rect.width && h == Rect.height )
		return;
	if( w >= 1 )
		Rect.width = w;
	if( h >= 1 )
		Rect.height = h;
}


void Widget::redraw( )
{
	if( !visible )
		return;

	extern MainConfig conf;
	float startx, starty, width, height;
	if(Parent){
		width = Parent->getWidgetWidth( );
		height = Parent->getWidgetHeight( );
		startx = Parent->getWidgetRealX( );
		starty = Parent->getWidgetRealY( ) + height;
	}else{
		startx = 0;
		starty = (float)conf.video.windowHeight;
		width = (float)conf.video.windowWidth;
		height = (float)conf.video.windowHeight;
	}
	Position.x = startx + Rect.x;
	Position.y = starty - Rect.y;
	switch( Align & (waLEFT | waRIGHT) ){
		case waMIDDLE:
			Position.x += 0.5f * ( width - this->Rect.width );
			//posx = startx + width * 0.5f - this->Rect.width * 0.5f + Rect.x;
			break;
		case waRIGHT:
			Position.x += width - this->Rect.width;
			//posx = startx + width - this->Rect.width + Rect.x;
			break;
		case waLEFT:
		default:
			//posx = startx + Rect.x;
			break;
	}
	switch( Align & (waTOP | waBOTTOM) ){
		case waMIDDLE:
			Position.y -= 0.5f * ( height  + this->Rect.height );
			//posy = starty - height * 0.5f - this->Rect.height * 0.5f - Rect.y;
			break;
		case waBOTTOM:
			Position.y -= height;
			//posy = starty - height - Rect.y;
			break;
		case waTOP:
		default:
			Position.y -= this->Rect.height;
			//posy = starty - this->Rect.height - Rect.y;
			break;
	}
	//Rect.x = posx;
	//Rect.y = posy;
	if( background ){
		background->resize( Rect.width, Rect.height, 0 );
		background->setPosition( Position.x, Position.y, getZ() );
	}

}


float Widget::getZ( )
{
	extern MainConfig conf;
	return Position.z + conf.widgetsPosZ;
}


void Widget::setParent( Widget* p )
{
	extern MainConfig conf;
	Parent = p;
	Position.z = Position.z + p->getZ( ) - conf.widgetsPosZ + 0.1f;
	redraw();
}


Widget* Widget::getChild( std::string name )
{
	listElement<Widget*>* l = Children.head;
	while( l != NULL ){
		Widget* w = l->data;
		if( w->getWidgetName() == name )
			return l->data;
		l = l->next;
	}
	return NULL;
}

void Widget::addChild( Widget* child )
{
	if( child == NULL )
		return;
	Children.push( child );
	Widget* cp = child->getParent();
	if( cp != NULL )
		cp->removeChild( child );
	child->setParent( this );
}

void Widget::removeChild( Widget* child )
{
	if( child == NULL )
		return;
	listElement< Widget* >* lelem = Children.head;
	while( lelem != NULL ){
		if( lelem->data == child )
			break;
		lelem = lelem->next;
	}
	Children.remove( lelem );
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
	redraw();
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

