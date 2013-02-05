/*
 * Text.cpp
 *
 *  Created on: 31.03.2012
 *
 */

#include "graphics/Text.h"
#include "graphics/Render.h"
#include "graphics/Font.h"

#include "config.h"
#include "debug.h"
#include "hacks.h"
#include <map>

#include "safestring.h" // strdup



#define ITER_SPRITES ITER_LIST( Sprite*, sprites )

static std::map < std::string, std::map< int, font_data* > > LoadedFonts;


void clean_fonts( )
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

	ftDone();
}

bool load_ttfont( std::string dir, std::string name, int size )
{
	std::string filename = dir + name;
	font_data* font = new font_data();


	if( !font->load( filename.c_str() , size ) ){
		delete font;
		return false;
	}

	LoadedFonts[name][size] = font;
	return true;
}


font_data* get_font( std::string name, int size  )
{
	extern MainConfig conf;
	if( !LoadedFonts.count(name) || !LoadedFonts[name].count(size) ){
		if( !load_ttfont( conf.path.fonts, name, size ) ){
			Debug::debug( Debug::GRAPHICS, "Cannot load font %s.\n", name.c_str() );
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


int Text::getLineSize( )
{
	return (int)((float)font->cellHeight * lineHeight);
}


void Text::setFont( const char* name, int size )
{
	font = get_font( name, size );
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
		it->data->move( dx, dy, dz );
	}
}


Sprite* create_sprite( s3f& position, Char* c, font_data* font )
{
	Sprite* s = RenderManager::CreateGLSprite( position.x, position.y, position.z,
						font->cellWidth, font->cellHeight, font->texture, c->pic );
	s->clearLight();
	return s;
}


void Text::setText(const char* str)
{
	float lineheight;

	if( !font )
		return;

	if( (content && str) && !strcmp( str, content ) )
		return;

	//clear();
	Width = Height = 0;

	if( content )
		free(content);
	content = strdup( str );

	char* text = strdup( str );

	listElement< Sprite* >* slist = sprites.head;


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

			s3f pos(position.x + tmpwidth + kerning,
					position.y - tmpheight,
					position.z);

			if( slist != NULL ){
				slist->data->setPosition( &pos );
				slist->data->setPicture( tmpc->pic );
				slist->data->setVisible();
				slist = slist->next;
			}else{
				Sprite* s = create_sprite( pos, tmpc, font );
				s->brush.set_color( color );
				sprites.push_back(s);
			}

			tmpwidth += kerning + tmpc->horiAdvance;
			prev = tmpc->index;
		}
		++Lines;
		Width += tmpwidth;
		token = strtok(NULL, "\n");
	}
	Height = (int)(Lines * lineheight);
	free(text);

	// Hide sprites
	while( slist != NULL ){
		RenderManager::FreeGLSprite( slist->data );
		slist = sprites.remove( slist );
	}


	// FIXME: 1>yukkuri\graphics\text.cpp(140): warning C4800: 'unsigned char' :
	//          forcing value to bool 'true' or 'false' (performance warning)
	setVisible( (bool)isVisible() );
	setFixed( (bool)isFixed() );
}

void Text::setColor( int r, int g, int b )
{
	color.set( r, g, b, 255 );
	ITER_SPRITES{
		it->data->brush.set_color( color );
	}
}

#define SETSPRITEFLAG(name, flag)			\
void Text::set##name( bool v )				\
{											\
	if( v ){								\
		flags |= flag;						\
		ITER_SPRITES it->data->set##name();	\
	}else{									\
		flags &= ~flag;						\
		ITER_SPRITES it->data->clear##name();	\
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


void Text::setCursorPosition( unsigned int pos )
{
	if(!font)
		return;
	cursorPosition = pos;
	// \n was not added when string was build so remove it
	pos += 1 - Lines;

	listElement<Sprite*>* prev = sprites.head;
	for( UINT i = 0; i < pos; ++i ){
		if(!prev->next){
			cursorPosition = i - 1;
			break;
		}
		prev = prev->next;
	}
	if( !prev || prev->data->brush.type != prQUADS )
		return;

	if( cursor == NULL ){
			Char* tmpc = font->getChar('|');
			cursorBearing = (tmpc->metrics.horiBearingX >> 6) + 2;
			cursor = RenderManager::CreateGLSprite(
					position.x, position.y, position.z,
					font->cellWidth, font->cellHeight, font->texture, tmpc->pic );
			cursor->setFixed();
			cursor->clearLight();
		cursor->setVisible();
	}

	s3f& position = prev->data->brush.vertex_origin;
	cursor->setPosition( position.x - cursorBearing, position.y, position.z );
}
