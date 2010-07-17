/*
 * Interface.cpp
 *
 *  Created on: 30.06.2010
 */

#include "Interface.h"

UI UI::yui; // :3

UI::~UI( )
{
	for(std::vector< Widget* >::iterator it = widgets.begin(); it != widgets.end(); it++){
		delete (*it);
	}
}


void UI::LoadAllWidgets( )
{
	std::vector< string > v;
	LuaConfig::Instance()->getSubconfigs("widget", v);
	for(std::vector <string>::iterator it = v.begin(); it != v.end(); ++it ){
		LoadWidget( (*it) );
	}

}

Widget* UI::LoadWidget( string name )
{
	Widget* w;
	LuaConfig* conf = LuaConfig::Instance();

	//FIXME: shi! This fufuuunction is so HUUUUUUUUUUUUUUUUUUUGE.

	debug(5, "Loading widget " + name + "\n");

	//Prevent loop inheritance.
	if( GetWidget( name ) ){
		debug(5, "Widget with name " + name + " already exists\n");
		return NULL;
	}

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

	conf->getValue("name", name, "widget", w->name );
	conf->getValue("x", name, "widget", w->posx );
	conf->getValue("y", name, "widget", w->posy );
	conf->getValue("width", name, "widget", w->width );
	conf->getValue("height", name, "widget", w->height );

	int z = 0;
	conf->getValue("depth", name, "widget", z );
	w->setZ( z );


	if(w->getType() != NONE){
		string imgname;
		string text;
		int bgx, bgy;
		conf->getValue("bgimage", name, "widget", imgname );
		conf->getValue("text", name, "widget", text );
		conf->getValue("bgposx", name, "widget", bgx );
		conf->getValue("bgposy", name, "widget", bgy );

		if(!w->create(imgname, text, bgx, bgy)){
			return NULL;
		}

		if( type == TEXT || type == BAR ){
			string font;
			float textx, texty;
			textx = texty = 0;
			int fontsize = 12;
			vector<int> vcolor;

			conf->getValue("textx", name, "widget", textx );
			conf->getValue("texty", name, "widget", texty );
			conf->getValue("font", name, "widget", font );
			conf->getValue("fontsize", name, "widget", fontsize );
			conf->getValue("fontcolor", name, "widget", vcolor );

			w->setFont(font, fontsize);
			w->setTextPosition(textx, texty);
			if( vcolor.size( ) > 2 )
				w->setFontColor(vcolor[0], vcolor[1], vcolor[2]);

			if( type == BAR ){
				//int topimgx, topimgy;
				int position[9];
				vcolor.clear();
				//Order: topimgx, topimgy, barx, bary, barwidth, barheight
				//Ya, it's cruve, but it's simple
				conf->getValue( "topimgx", name, "widget", position[0] );
				conf->getValue( "topimgy", name, "widget", position[1] );
				conf->getValue( "barx", name, "widget", position[2] );
				conf->getValue( "bary", name, "widget", position[3] );
				conf->getValue( "barwidth", name, "widget", position[4] );
				conf->getValue( "barheight", name, "widget", position[5] );
				conf->getValue( "source", name, "widget", imgname );
				conf->getValue( "barcolor", name, "widget", vcolor );
				if( vcolor.size( ) > 2 ){
					position[6] = vcolor[0]; //r
					position[7] = vcolor[1]; //g
					position[8] = vcolor[2]; //b
				}else{
					position[6] = 0; position[7] = 0; position[8] = 0;
				}
				w->createBar( imgname, position );
			}
		}
	}

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

Widget* UI::GetWidget( string name )
{
	for (std::vector< Widget* >::iterator it=widgets.begin() ; it != widgets.end(); it++ ){
		if( (*it)->name == name )
			return (*it);
	}
	return NULL;
}

void UI::DrawWidgets( )
{
	for (std::vector< Widget* >::iterator it=widgets.begin() ; it != widgets.end(); it++ )
		(*it)->draw();
}


