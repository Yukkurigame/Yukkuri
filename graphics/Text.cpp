/*
 * Text.cpp
 *
 *  Created on: 31.03.2012
 *
 */

#include "Text.h"
#include "Render.h"
#include "config.h"
#include "debug.h"
#include <map>

static std::map < std::string, std::map< int, font_data* > > LoadedFonts;

bool LoadTTFont( std::string dir, std::string name, int size )
{
	font_data* font;
	std::string filename;

	filename = dir + name + ".ttf";

	font = new font_data();
	//FIXME: it's cruved;
	if( font->load( filename.c_str() , size ) ){
		LoadedFonts[name][size] = font;
	}else{
		delete font;
		return false;
	}
	return true;
}


font_data* GetFont( std::string name, int size  )
{
	extern MainConfig conf;
	if( !LoadedFonts.count(name) || !LoadedFonts[name].count(size) ){
		if( !LoadTTFont( conf.fontsPath, name, size ) ){
			Debug::debug( Debug::GRAPHICS, "Cannot load font " + name + ".\n" );
			return NULL;
		}
	}
	return LoadedFonts[name][size];
}


Text::Text( )
{
	font = NULL;
}


void Text::setFont( std::string name, int size )
{
	font = GetFont( name, size );
}

void Text::setPosition( float x, float y, float z )
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Text::setText(const char* str)
{
	int textlen;
	int lineheight;
	int nlines;

	if( !font )
		return;

	textlen = strlen( str );

	char* text = (char*)malloc( sizeof(char)*textlen + 1 );
	strcpy( text, str );

	char* token;
	nlines = 0;
	token = strtok( text, "\n" );
	lineheight = font->cellHeight + font->cellHeight / 4;
	while( token != 0 ){
		int tmpwidth = 0;
		int tmpheight = lineheight * nlines;
		char* line = token;
		for( unsigned int g = 0, e = strlen( line ); g < e; ++g ){
			Char* tmpc = font->getChar(static_cast<unsigned int>(line[g]));
			addSprite(tmpwidth, tmpheight, tmpc);
			tmpwidth += tmpc->horiAdvance;
		}
		++nlines;
		token = strtok(NULL, "\n");
	}
	free(text);
}


void Text::addSprite( int x, int y, Char* c )
{
	Sprite* s = RenderManager::Instance()->CreateGLSprite(
		position.x + static_cast<float>(x), position.y - static_cast<float>(y), position.z,
		font->cellWidth, font->cellHeight, font->texture, c->pic );
	s->fixed = fixed;
	//s->clr.r = x * font->cellWidth;
	sprites.push_back(s);
}
