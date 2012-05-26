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
#include "hacks.h"
#include <map>

#define ITER_SPRITES for( std::vector< Sprite* >:: iterator it = sprites.begin(), vend = sprites.end(); it != vend; ++it )

static std::map < std::string, std::map< int, font_data* > > LoadedFonts;


void CleanFonts( )
{
	Debug::debug( Debug::MAIN, "Cleaning fonts.\n" );
	FOREACHIT( LoadedFonts ){
		FOREACH( vit, it->second ){
			vit->second->clean();
			delete vit->second;
		}
		it->second.clear();
	}
	LoadedFonts.clear();
}

bool LoadTTFont( std::string dir, std::string name, int size )
{
	font_data* font;
	std::string filename;

	filename = dir + name;

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
	lineHeight = 1;
	Visible = true;
	Width = Height = 0;
}

Text::~Text( )
{
	clear();
}


void Text::setFont( std::string name, int size )
{
	font = GetFont( name, size );
}

void Text::setPosition( float x, float y, float z )
{
	float dx = x - position.x;
	float dy = y - position.y;
	float dz = z - position.z;
	position.x = x;
	position.y = y;
	position.z = z;
	ITER_SPRITES{
		(*it)->move( dx, dy, dz );
	}
}

void Text::setText(const char* str)
{
	int textlen;
	int lineheight;
	int nlines;

	if( !font )
		return;

	clear();

	textlen = strlen( str );

	char* text = (char*)malloc( sizeof(char)*textlen + 1 );
	strcpy( text, str );

	char* token;
	nlines = 0;
	token = strtok( text, "\n" );
	lineheight = font->cellHeight * lineHeight;
	while( token != 0 ){
		int tmpwidth = 0;
		int tmpheight = lineheight * nlines;
		char* line = token;
		FT_UInt prev = 0;
		for( unsigned int g = 0, e = strlen( line ); g < e; ++g ){
			Char* tmpc = font->getChar(static_cast<unsigned int>(line[g]));
			int kerning = font->getKerning(prev, tmpc->index);
			addSprite(tmpwidth + kerning, tmpheight, tmpc);
			tmpwidth += kerning + tmpc->horiAdvance;
			prev = tmpc->index;
		}
		++nlines;
		Width += tmpwidth;
		Height += tmpheight;
		token = strtok(NULL, "\n");
	}
	Height += lineheight;
	free(text);

	setVisible( Visible );
	setFixed( Fixed );
}

void Text::setColor( int r, int g, int b )
{
	color.set( r, g, b );
	ITER_SPRITES{
		(*it)->clr.set( r, g, b, 255 );
	}
}



void Text::setVisible( bool v )
{
	Visible = v;
	ITER_SPRITES{
		if( v )
			(*it)->setVisible();
		else
			(*it)->clearVisible();
	}
}
void Text::setFixed( bool f )
{
	Fixed = f;
	ITER_SPRITES{
		if( f )
			(*it)->setFixed();
		else
			(*it)->clearFixed();
	}
}

void Text::clear( )
{
	Width = Height = 0;
	RenderManager::Instance()->FreeGLSprites( &sprites );
}




void Text::addSprite( int x, int y, Char* c )
{
	Sprite* s = RenderManager::Instance()->CreateGLSprite(
		position.x + static_cast<float>(x), position.y - static_cast<float>(y), position.z,
		font->cellWidth, font->cellHeight, font->texture, c->pic );
	s->clr.set( &color );
	sprites.push_back(s);
}

