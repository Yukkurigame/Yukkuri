/*
 * Interface.cpp
 *
 *  Created on: 30.06.2010
 */

#include "Interface.h"
#include "widgets/WidgetText.h"
#include "widgets/WidgetBar.h"
#include "LuaConfig.h"

#include "hacks.h"
#include "debug.h"


namespace
{
	std::vector< Widget* > widgets;
	static unsigned int LastWidgetId = 1;


	void DeleteWidget( std::string name ){
		//FIXME: why this function is empty?
	}

}



void Interface::clean( )
{
	Debug::debug( Debug::INTERFACE, "Remove widgets.\n" );

	FOREACHIT( widgets ){
		delete (*it);
	}
}


void Interface::LoadAllWidgets( )
{
	std::vector< std::string > v;
	LuaConfig* lc = new LuaConfig;
	lc->getSubconfigsList( "widget", v );
	FOREACHIT( v ){
		LoadWidget( (*it) );
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

	std::string type;
	conf->getValue( "type", id, type );

	if( type == "Widget" ){
		w = new Widget( );
		w->setType( BLANK );
	}else if( type == "TextWidget" ){
		w = new WidgetText( );
		w->setType( TEXT );
	}else if( type == "BarWidget" ){
		w = new WidgetBar( );
		w->setType( BAR );
	}else{
		debug( Debug::INTERFACE, "Widget with id " + id + " have bad type " + type + ".\n" );
		w = new Widget( );
		w->setType( NONE );
	}

	if( !w->load( id ) ){
		delete w;
		return NULL;
	}

	w->setId( LastWidgetId++ );

	//Add in cache;
	widgets.push_back(w);

	std::vector < std::string > childs;
	conf->getValue("children", id, childs);
	FOREACHIT( childs ){
		Widget* cld = LoadWidget( *it );
		if(cld){
			w->addChild(cld);
			cld->setParent(w);
		}
	}

	delete conf;

	return w;
}

Widget* Interface::GetWidget( unsigned int id )
{
	if( id > 1 && id < LastWidgetId  ){
		FOREACHIT( widgets ){
			if( (*it)->getId( ) == id )
				return (*it);
		}
	}
	return NULL;
}

Widget* Interface::GetWidget( std::string name, Widget* parent )
{
	if( parent != NULL )
		return parent->getChildren(name);
	FOREACHIT( widgets ){
		if( (*it)->getParent() == NULL && (*it)->getName() == name )
			return (*it);
	}
	return NULL;
}

void Interface::Update( )
{
	FOREACHIT( widgets )
		(*it)->Update( );
}
