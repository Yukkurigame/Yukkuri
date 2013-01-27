/*
 * Textures.cpp
 *
 *  Created on: 26.01.2013
 */


#include "graphics/render/Textures.h"

#include "debug.h"


namespace Textures {

	//////////////////////////////////////////////////
	// Textures

	UINT textures_count = 0;
	TextureInfo* textures = NULL;


	void resize( UINT count ){
		textures_count += count;
		textures = (TextureInfo*)realloc(textures, (UINT)sizeof(TextureInfo) * textures_count );
	}

}


/* Remove all allocated textures */
void Textures::clean( )
{
	if( !textures )
		return;

	for( unsigned int i = 0; i < textures_count; ++i ){
		delete[] textures[i].id;
		glDeleteTextures( 1, &textures[i].atlas );
		glDeleteTextures( 1, &textures[i].normals );
	}
	free( textures );
}


/*	This function creates new opengl texture. If texture is exists it will be cleared.
 *	ahandle - pointer to texture id
 *	width, height - texture size
 *	returns boolean
 */
bool Textures::create( GLuint* ahandle, GLenum target, GLint internalformat,
		GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
	// Texture generation and setup.
	if( ahandle == NULL ){
		Debug::debug( Debug::GRAPHICS, "Bad texture for generation.\n" );
		return false;
	}
	if( *ahandle == 0 )
		glGenTextures(1, ahandle);
	glBindTexture( target, *ahandle );
	glTexImage2D( target, 0, internalformat, width, height, 0, format, type, pixels );
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri( target, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameteri( target, GL_TEXTURE_WRAP_T, GL_CLAMP );
	glBindTexture( target, 0 );
	return true;
}





UINT Textures::get_active( UINT id )
{
}


TextureInfo* Textures::get_pointer( UINT number )
{
	if( number < textures_count )
		return &textures[number];
	return NULL;
}




//////////////////////////////////////////////////
// Textures


int Textures::push( TextureProxy* proxy, GLuint atlas, GLuint normals )
{
	int count = 1;
	resize( 1 );
	TextureInfo& ti = textures[count];
	ti.fromTextureProxy( proxy, atlas );
	ti.id = new char[proxy->id.size() + 1];
	ti.normals = normals;
	strcpy(ti.id, proxy->id.c_str());
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


bool Textures::LoadTextures( )
{
	LuaConfig* lc = new LuaConfig;
	std::vector< std::string > names;
	int textures_count;
	std::string config = "sprite";

	lc->getSubconfigsLength(config, textures_count);

	Debug::debug( Debug::GRAPHICS, citoa(textures_count) + " textures found.\n" );

	if( !textures_count ){
		delete lc;
		return false;
	}

	lc->getSubconfigsList(config, names);

	FOREACHIT( names )
		TextureAtlas::addTexture( *it );

	delete lc;
	// load basic textures to main atlas
	return TextureAtlas::create( &atlasHandle, &normalsHandle, atlasWidth, atlasHeight );
}





int Textures::GetTextureNumberById( std::string id )
{
	for( unsigned int i = 0; i < textures_count; ++i ){
		if( id.compare(textures[i].id) == 0 )
			return i;
	}
	return 0;
}




