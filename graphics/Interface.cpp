/*
 * Interface.cpp
 *
 *  Created on: 30.06.2010
 */

#include "Interface.h"
#include "Luaconfig.h"

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
	std::vector< string > v;
	LuaConfig::Instance()->getSubconfigsList("widget", v);
	for(std::vector <string>::iterator it = v.begin(); it != v.end(); ++it ){
		LoadWidget( (*it) );
	}
}

Widget* UI::LoadWidget( string name )
{
	Widget* w;
	LuaConfig* conf = LuaConfig::Instance();

	debug(5, "Loading widget " + name + "\n");

	//Prevent loop inheritance.
	//w = GetWidget( name );
	//if( w ){
		//debug(5, "Widget with name " + name + " already exists.\n");
		//return w;
	//}

	int type;
	conf->getValue("type", name, "widget", type );

	switch(type){
		case BLANK:
			w = new Widget( );
			w->setType( BLANK );
			break;
		case TEXT:
			w = new TextWidget( );
			w->setType( TEXT );
			break;
		case BAR:
			w = new BarWidget( );
			w->setType( BAR );
			break;
		default:
			w = new Widget( );
			w->setType( NONE );
			break;
	}

	if( !w->load( name ) ){
		delete w;
		return NULL;
	}

	w->setId( LastWidgetId++ );

	//Add in cache;
	widgets.push_back(w);

	std::vector < string > childs;
	conf->getValue("children", name, "widget", childs);
	for(std::vector <string>::iterator it = childs.begin(); it != childs.end(); ++it ){
		Widget* cld = LoadWidget( (*it) );
		if(cld){
			w->addChild(cld);
			cld->setParent(w);
		}
	}

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

Widget* UI::GetWidget( string name )
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
