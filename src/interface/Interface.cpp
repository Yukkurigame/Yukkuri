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

	Widget* NewWidget( const char* id, enum wType type ){
		Widget* w = NULL;
		switch( type ){
			default:
				debug( Debug::INTERFACE, "Widget with id %s have bad type: %d.\n", id, type );
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
		if( w != NULL ){
			if( w->create( id ) )
				w->setType( type );
			else
				delete w, w = NULL;
		}
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


Widget* Interface::CreateWidget( const char* id, UINT type )
{
	Widget* w;
	debug( Debug::INTERFACE, "Creating widget %s.\n", id );
	w = NewWidget( id, static_cast<enum wType>(type) );
	if( w != NULL ){
		w->setId( LastWidgetId++ );
		widgets.push_back( w );
	}
	return w;
}

/*
void Interface::LoadAllWidgets( )
{
	list< char* > v;
	LuaConfig* lc = new LuaConfig;
	lc->getSubconfigsList( "widget", v );
	ITER_LIST( char*, v ){
		LoadWidget( it->data );
	}
	delete lc;
}


Widget* Interface::LoadWidget( const char* id )
{
	Widget* w;
	LuaConfig* conf = new LuaConfig;

	debug( Debug::INTERFACE, "Loading widget %s.\n", id );

	//Prevent loop inheritance.
	//w = GetWidget( name );
	//if( w ){
		//debug( Debug::INTERFACE, "Widget with name " + name + " already exists.\n");
		//return w;
	//}

	std::string stype;
	enum wType type = wtNone;
	conf->getValue( "type", id, stype );
	if( stype == "Widget" ){
		type = wtBlank;
	}else if( stype == "TextWidget" ){
		type = wtText;
	}else if( stype == "BarWidget" ){
		type = wtBar;
	}

	w = NewWidget( id, type );
	if( w == NULL )
		return w;

	if( !w->load( id ) ){
		delete w;
		return NULL;
	}

	w->setId( LastWidgetId++ );

	//Add in cache;
	widgets.push_back(w);

	list< char* > childs;
	conf->getValue( "children", id, childs );
	ITER_LIST( char*, childs ){
		Widget* cld = LoadWidget( it->data );
		w->addChild( cld );
		free( it->data );
	}

	delete conf;

	return w;
}
*/

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
