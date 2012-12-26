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

#include "safestring.h" // strdup

#define ITER_SPRITES FOREACHIT( sprites )

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
	cursor = NULL;
	lineHeight = 1;
	flags = 1; // Visible only
	Lines = 0;
	cursorBearing = 0;
	cursorPosition = 0;
	Width = Height = 0;
	content = NULL;
}

Text::~Text( )
{
	clear();
	RenderManager::FreeGLSprite( cursor );
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
	float lineheight;

	if( !font )
		return;

	if( (content && str) && !strcmp( str, content ) )
		return;

	clear();

	if( content )
		free(content);
	content = strdup( str );

	char* text = strdup( str );

	char* token;
	Lines = 0;
	token = strtok( text, "\n" );
	lineheight = font->cellHeight * lineHeight;
	while( token != 0 ){
		int tmpwidth = 0;
		int tmpheight = (int)lineheight * Lines;
		char* line = token;
		FT_UInt prev = 0;
		for( unsigned int g = 0, e = strlen( line ); g < e; ++g ){
			Char* tmpc = font->getChar(static_cast<unsigned int>(line[g]));
			if(!tmpc)
				continue;
			int kerning = font->getKerning(prev, tmpc->index);
			addSprite(tmpwidth + kerning, tmpheight, tmpc);
			tmpwidth += kerning + tmpc->horiAdvance;
			prev = tmpc->index;
		}
		++Lines;
		Width += tmpwidth;
		token = strtok(NULL, "\n");
	}
	Height = (int)(Lines * lineheight);
	free(text);

	// FIXME: 1>yukkuri\graphics\text.cpp(140): warning C4800: 'unsigned char' :
	//          forcing value to bool 'true' or 'false' (performance warning)
	setVisible( (bool)isVisible() );
	setFixed( (bool)isFixed() );
}

void Text::setColor( int r, int g, int b )
{
	color.set( r, g, b, 255 );
	ITER_SPRITES{
		(*it)->brush.set_color( color );
	}
}

#define SETSPRITEFLAG(name, flag)			\
void Text::set##name( bool v )				\
{											\
	if( v ){								\
		flags |= flag;						\
		ITER_SPRITES (*it)->set##name();	\
	}else{									\
		flags &= ~flag;						\
		ITER_SPRITES (*it)->clear##name();	\
	}										\
}

// Some macro for flags
SETSPRITEFLAG(Visible, 1)
SETSPRITEFLAG(Fixed, 2)

#undef SETSPRITEFLAG


void Text::clear( )
{
	Width = Height = 0;
	if( content ){
		free(content);
		content = NULL;
	}
	RenderManager::FreeGLSprites( &sprites );
}


void Text::addSprite( int x, int y, Char* c )
{
	Sprite* s = RenderManager::CreateGLSprite( position.x + static_cast<float>(x),
		position.y - static_cast<float>(y), position.z, font->cellWidth,
		font->cellHeight, font->texture, c->pic );
	s->brush.set_color( color );
	sprites.push_back(s);
}

void Text::setCursorPosition( unsigned int pos )
{
	if(!font)
		return;
	cursorPosition = pos;
	// \n was not added when string was build so remove it
	pos += 1 - Lines;
	if( cursor == NULL ){
		Char* tmpc = font->getChar('|');
		cursorBearing = (tmpc->metrics.horiBearingX >> 6) + 2;
		cursor = RenderManager::CreateGLSprite(
				position.x, position.y, position.z,
				font->cellWidth, font->cellHeight, font->texture, tmpc->pic );
		cursor->setFixed();
	}
	Sprite* next = NULL;
	if( pos >= sprites.size() ){
		next = sprites.back();
		cursorPosition = sprites.size() - 1;
	}else{
		next = sprites[pos];
	}
	if(next){
		s3f* lb = next->brush.lb();
		cursor->setPosition( lb->x - cursorBearing, lb->y );
	}
	cursor->setVisible();
}
