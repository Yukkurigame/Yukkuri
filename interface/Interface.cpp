/*
 * Interface.cpp
 *
 *  Created on: 30.06.2010
 */

#include "Interface.h"
#include "widgets/TextWidget.h"
#include "widgets/BarWidget.h"
#include "Luaconfig.h"

#include "debug.h"


UI UI::yui; // :3

static unsigned int LastWidgetId = 1;



UI::~UI( )
{
	for(std::vector< Widget* >::iterator it = widgets.begin(); it != widgets.end(); it++){
		delete (*it);
	}
}

void UI::LoadAllWidgets( )
{
	std::vector< std::string > v;
	LuaConfig* lc = new LuaConfig;
	lc->getSubconfigsList( "widget", v );
	for(std::vector < std::string >::iterator it = v.begin(); it != v.end(); ++it ){
		LoadWidget( (*it) );
	}
	delete lc;
}

Widget* UI::LoadWidget( std::string id )
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
		w = new TextWidget( );
		w->setType( TEXT );
	}else if( type == "BarWidget" ){
		w = new BarWidget( );
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
	for(std::vector < std::string >::iterator it = childs.begin(); it != childs.end(); ++it ){
		Widget* cld = LoadWidget( (*it) );
		if(cld){
			w->addChild(cld);
			cld->setParent(w);
		}
	}

	delete conf;

	return w;
}

Widget* UI::GetWidget( unsigned int id )
{
	if( id > 1 && id < LastWidgetId  ){
		for (std::vector< Widget* >::iterator it=widgets.begin() ; it != widgets.end(); it++ ){
			if( (*it)->getId( ) == id )
				return (*it);
		}
	}
	return NULL;
}

Widget* UI::GetWidget( std::string name )
{
	for (std::vector< Widget* >::iterator it=widgets.begin() ; it != widgets.end(); it++ ){
		if( (*it)->getName() == name )
			return (*it);
	}
	return NULL;
}

void UI::Update( )
{
	for (std::vector< Widget* >::iterator it=widgets.begin() ; it != widgets.end(); it++ ){
		(*it)->Update( );
	}
}
