/*
 * Interface.cpp
 *
 *  Created on: 30.06.2010
 */

#include "Interface.h"
#include "widgets/WidgetText.h"
#include "widgets/WidgetBar.h"
#include "scripts/LuaConfig.h"
#include "graphics/daytime.h"

#include "hacks.h"
#include "debug.h"
#include "safestring.h"


namespace
{
	std::vector< Widget* > widgets;
	static unsigned int LastWidgetId = 1;


	void DeleteWidget( std::string name ){
		//FIXME: why this function is empty?
	}

	Widget* NewWidget( std::string id, enum wType type ){
		Widget* w = NULL;
		switch( type ){
			default:
				debug( Debug::INTERFACE, "Widget with id " + id + " have bad type " + citoa(type) + ".\n" );
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

	FOREACHIT( widgets ){
		delete (*it);
	}
}


Widget* Interface::CreateWidget( std::string id, enum wType type )
{
	Widget* w;
	debug( Debug::INTERFACE, "Creating widget " + id + "\n" );
	w = NewWidget(id, type);
	if( w != NULL ){
		w->setId( LastWidgetId++ );
		widgets.push_back(w);
	}
	return w;
}


void Interface::LoadAllWidgets( )
{
	list< std::string > v;
	LuaConfig* lc = new LuaConfig;
	lc->getSubconfigsList( "widget", v );
	ITER_LIST( std::string, v ){
		LoadWidget( it->data );
	}
	delete lc;
}


Widget* Interface::LoadWidget( std::string id )
{
	Widget* w;
	LuaConfig* conf = new LuaConfig;

	debug( Debug::INTERFACE, "Loading widget " + id + "\n" );

	//Prevent loop inheritance.
	//w = GetWidget( name );
	//if( w ){
		//debug( Debug::INTERFACE, "Widget with name " + name + " already exists.\n");
		//return w;
	//}

	std::string stype;
	enum wType type = wtNone;
	conf->getValue( "type", id.c_str(), stype );
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

	std::vector < std::string > childs;
	conf->getValue("children", id.c_str(), childs);
	FOREACHIT( childs ){
		Widget* cld = LoadWidget( *it );
		w->addChild(cld);
	}

	delete conf;

	return w;
}

Widget* Interface::GetWidget( unsigned int id )
{
	if( id > 0 && id < LastWidgetId  ){
		FOREACHIT( widgets ){
			if( (*it)->getWidgetId( ) == id )
				return (*it);
		}
	}
	return NULL;
}

Widget* Interface::GetWidget( std::string name, Widget* parent )
{
	if( parent != NULL )
		return parent->getChild( name );
	FOREACHIT( widgets ){
		if( (*it)->getParent() == NULL && (*it)->getWidgetName() == name )
			return (*it);
	}
	return NULL;
}

/*
void Interface::Update( )
{
	FOREACHIT( widgets )
		(*it)->Update( );
}
*/
