/*
 * Textures.cpp
 *
 *  Created on: 26.01.2013
 */


#include "graphics/render/Textures.h"
#include "graphics/render/Atlas.h"
#include "graphics/render/GLTextures.h"
#include "debug.h"

#include "scripts/LuaConfig.h"
#include "safestring.h"
#include "hacks.h"


namespace Textures {

	//////////////////////////////////////////////////
	// Atlases

	int atlasWidth, atlasHeight;
	GLuint atlasHandle;
	GLuint normalsHandle;


	//////////////////////////////////////////////////
	// Textures

	UINT textures_count = 0;
	TextureInfo* textures = NULL;


	void resize( UINT count ){
		textures_count += count;
		textures = (TextureInfo*)realloc(textures, (UINT)sizeof(TextureInfo) * textures_count );
	}

	//////////////////////////////////////////////////
	// Active textures
	UINT active_limit = 16;
	GLuint* active_textures = NULL;
	int next_active = 0;

}

void Textures::init( )
{
	//glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &active_limit );
	active_textures = (GLuint*)malloc( sizeof(GLuint) * active_limit );
	memset( active_textures, 0, sizeof(GLuint) * active_limit );
}


/* Remove all allocated textures */
void Textures::clean( )
{
	if( !textures )
		return;

	for( unsigned int i = 0; i < textures_count; ++i )
		free(textures[i].id);
	free( textures );
}


TextureInfo* Textures::get_pointer( UINT number )
{
	if( number < textures_count )
		return &textures[number];
	return NULL;
}

UINT Textures::get_by_name( const char* name )
{
	for( unsigned int i = 0; i < textures_count; ++i ){
		if( strcmp( name, textures[i].id ) == 0 )
			return i;
	}
	return 0;
}

/*
UINT Textures::get_active( UINT id )
{
	int blank = -1;
	for( int i = 0; i < active_limit; ++i ){
		if( active_textures[i] == id )
			return i;
		else if( blank < 0 && !active_textures[i] )
			blank = i;
	}

	int active;
	if( blank < 0 ){
		active = next_active;
		if( ++next_active >= active_limit )
			next_active = 0;
	}else{
		active = blank;
	}

	// Bind texture
	active_textures[active] = id;
	glActiveTexture( GL_TEXTURE0 + active );
	glBindTexture( GL_TEXTURE_2D, id );

	return active;
}
*/

/* Bind texture to active location
 * id - texture id
 * location - active location
 * target - binding target
 */
void Textures::bind( UINT id, UINT location, GLenum target )
{
	if( location > active_limit ||
		active_textures[location] == id )
		return;
	active_textures[location] = id;
	glActiveTexture( GL_TEXTURE0 + location );
	glBindTexture( target, id );
}


void Textures::unbind( UINT id, GLenum target )
{
	for( UINT i = 0; i < active_limit; ++i ){
		if( active_textures[i] != id )
			continue;

		// Unbind texture from active slot
		glActiveTexture( GL_TEXTURE0 + i );
		glBindTexture( target, 0 );
		active_textures[i] = 0;
	}
}


// Unbind all textures
void Textures::unbind( )
{
	for( UINT i = 0; i < active_limit; ++i ){
		if( !active_textures[i] )
			continue;
		// Unbind texture from active slot
		glActiveTexture( GL_TEXTURE0 + i );
		glBindTexture( GL_TEXTURE_2D, 0 );
		active_textures[i] = 0;
	}
}


void Textures::apply( list<GLuint>* textures, int& flags )
{
	listElement< GLuint >* texture = textures->head;

	if( !texture ){
		if( flags & 1 ){
			glDisable( GL_TEXTURE_2D );
			flags &= ~1;
		}
		// No textures needs to be applied
		return;
	}else if( !(flags & 1) ){
		glEnable( GL_TEXTURE_2D );
		flags |= 1;
	}

	for( int index = 0; texture != NULL; texture = texture->next, ++index ){
		bind( texture->data, index );
	}
}


UINT Textures::push( TextureProxy* proxy, GLuint atlas, GLuint normals )
{
	UINT count = textures_count;
	resize( 1 );
	TextureInfo& ti = textures[count];
	ti.fromTextureProxy( proxy, atlas );
	ti.id = strdup(proxy->id);
	ti.normals = normals;
	return count;
}


void Textures::push( list< TextureProxy* >& tarray, GLuint atlas, GLuint normals )
{
	listElement< TextureProxy* >* t = tarray.head;
	while( t != NULL ){
		push( t->data, atlas, normals );
		t = t->next;
	}
}


bool Textures::load( )
{
	LuaConfig* lc = new LuaConfig;
	list< char* > names;
	int textures_count;
	const char* config = "sprite";

	lc->getSubconfigsLength( config, textures_count );

	Debug::debug( Debug::GRAPHICS, "Textures found: %d.\n", textures_count );

	if( !textures_count ){
		delete lc;
		return false;
	}

	lc->getSubconfigsList( config, names );

	ITER_LIST( char*, names ){
		TextureAtlas::addTexture( it->data );
		free( it->data );
	}

	delete lc;
	// load basic textures to main atlas
	return TextureAtlas::create( &atlasHandle, &normalsHandle, atlasWidth, atlasHeight );
}



void Textures::dump( TextureDump* output, UINT id )
{
	TextureInfo* tex = get_pointer( id );
	output->texture.fromTextureInfo(tex);
	output->texture.id = strdup( tex->id );
	output->width = (float)tex->swidth / (float)tex->twidth;
	output->height = (float)tex->sheight / (float)tex->theight;
	output->size = sizeof(char) * output->width * output->height * 4;
	output->data = (char*)malloc( output->size );

	bind( tex->atlas, 0 );
	glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, output->data );
	unbind( tex->atlas );
}


UINT Textures::load( TextureDump* input )
{
	UINT count = textures_count;
	resize( 1 );
	TextureInfo& ti = textures[count];
	ti.fromTextureInfo( &input->texture );
	ti.id = strdup( input->texture.id );
	ti.atlas = 0;

	GLTextures::generate( &ti.atlas, input->width, input->height, GL_UNSIGNED_BYTE,
			input->data );
	return count;
}
