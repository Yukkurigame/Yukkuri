/*
 * Interface.cpp
 *
 *  Created on: 30.06.2010
 */

#include "Interface.h"

UI UI::yui; // :3

UI::~UI( )
{
	for(std::map< string, Widget*>::iterator it = widgets.begin(); it != widgets.end(); it++){
		delete (*it).second;
	}
}


void UI::LoadAllWidgets( )
{
	std::vector< string > v;
	LuaConfig::conf.getSubconfigs("widget", v);
	for(std::vector <string>::iterator it = v.begin(); it != v.end(); ++it ){
		LoadWidget( (*it) );
	}

}

Widget* UI::LoadWidget( string name )
{
	Widget* w;

	debug(5, "Loading widget " + name + "\n");

	//Prevent loop inheritance.
	if( GetWidget( name ) ){
		debug(5, "Widget with name " + name + "already exists\n");
		return NULL;
	}

	int type;
	LuaConfig::conf.getValue("type", name, "widget", type );

	switch(type){
		case BLANK:
			w = new Widget( );
			w->setType( BLANK );
			break;
		case TEXT:
			w = new TextWidget( );
			w->setType( TEXT );
			break;
		default:
			w = new Widget( );
			w->setType( NONE );
			break;
	}

	LuaConfig::conf.getValue("name", name, "widget", w->name );
	LuaConfig::conf.getValue("x", name, "widget", w->posx );
	LuaConfig::conf.getValue("y", name, "widget", w->posy );
	LuaConfig::conf.getValue("width", name, "widget", w->width );
	LuaConfig::conf.getValue("height", name, "widget", w->height );


	if(w->getType() != NONE){
		string imgname;
		LuaConfig::conf.getValue("bg", name, "widget", imgname );

		if(!w->create(imgname)){
			return NULL;
		}
	}

	if( type == TEXT ){
		string font;
		float textx, texty;
		textx = texty = 0;
		int fontsize = 12;
		LuaConfig::conf.getValue("font", name, "widget", font );
		LuaConfig::conf.getValue("textx", name, "widget", textx );
		LuaConfig::conf.getValue("texty", name, "widget", texty );
		LuaConfig::conf.getValue("fontsize", name, "widget", fontsize );
		w->setFont(font, fontsize);
		w->setTextPosition(textx, texty);
		w->setText("");
	}

	//Add in cache;
	widgets[name] = w;

	std::vector < string > childs;
	LuaConfig::conf.getValue("children", name, "widget", childs);
	for(std::vector <string>::iterator it = childs.begin(); it != childs.end(); ++it ){
		Widget* cld = LoadWidget( (*it) );
		if(cld){
			w->addChild(cld);
			cld->setParent(w);
		}
	}

	return w;
}

Widget* UI::GetWidget( string name )
{
	map <string, Widget* >::iterator it;
	it = widgets.find( name );
	if( it != widgets.end() ){
		return it->second;
	}
	return NULL;
}

void UI::DrawWidgets( )
{
	for (std::map< string, Widget*>::iterator it=widgets.begin() ; it != widgets.end(); it++ )
		(*it).second->draw();
}


