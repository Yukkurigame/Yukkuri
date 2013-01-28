/*
 * Textures.cpp
 *
 *  Created on: 26.01.2013
 */


#include "graphics/render/Textures.h"
#include "graphics/render/TextureArray.h"
#include "graphics/render/Atlas.h"
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

}

/* Remove all allocated textures */
void Textures::clean( )
{
	if( !textures )
		return;

	for( unsigned int i = 0; i < textures_count; ++i )
		delete[] textures[i].id;
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


UINT Textures::get_active( UINT id )
{
	return 0;
}



UINT Textures::push( TextureProxy* proxy, GLuint atlas, GLuint normals )
{
	UINT count = textures_count;
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


bool Textures::load( )
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
