/*
 * Interface.cpp
 *
 *  Created on: 30.06.2010
 */

#include "Interface.h"
#include "widgets/Widget.h"
#include "widgets/WidgetText.h"
#include "widgets/WidgetBar.h"
#include "scripts/LuaConfig.h"
#include "utils/list.h"

#include "debug.h"
#include "safestring.h"


namespace
{
	list< Widget* > widgets;
	static unsigned int LastWidgetId = 1;


	void DeleteWidget( const char* name ){
		//FIXME: why this function is empty?
	}

	Widget* NewWidget( const char* name, enum wType type ){
		Widget* w = NULL;
		switch( type ){
			default:
				debug( Debug::INTERFACE, "Widget with id %s have bad type: %d.\n", name, type );
				type = wtNone;
				// No break. Create blank widget by default.
			case wtBlank:
				w = new Widget( );
				break;
			case wtText:
				w = new WidgetText( );
				break;
			case wtBar:
				w = new WidgetBar( );
				break;
		}

		if( w != NULL && !w->create( name, type ) )
			delete w, w = NULL;

		return w;
	}

}


void Interface::init( )
{
	//DayTime::loadInterface();
}


void Interface::clean( )
{
	Debug::debug( Debug::INTERFACE, "Remove widgets.\n" );

	ITER_LIST( Widget*, widgets ){
		delete it->data;
	}
}


Widget* Interface::CreateWidget( const char* name, UINT type )
{
	Widget* w;
	debug( Debug::INTERFACE, "Creating widget %s.\n", name );
	w = NewWidget( name, static_cast<enum wType>(type) );
	if( w != NULL ){
		w->setId( LastWidgetId++ );
		widgets.push_back( w );
	}
	return w;
}

Widget* Interface::GetWidget( UINT id )
{
	if( id > 0 && id < LastWidgetId  ){
		ITER_LIST( Widget*, widgets ){
			if( it->data->getWidgetId( ) == id )
				return it->data;
		}
	}
	return NULL;
}

Widget* Interface::GetWidget( const char* name, Widget* parent )
{
	if( parent != NULL )
		return parent->getChild( name );
	ITER_LIST( Widget*, widgets ){
		if( it->data->getParent() == NULL && it->data->getWidgetName() == name )
			return it->data;
	}
	return NULL;
}
