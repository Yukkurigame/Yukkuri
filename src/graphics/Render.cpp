/*
 * Render.cpp
 *
 *  Created on: 19.03.2012
 */
#include "Render.h"
#include "graphics/gl_extensions.h"
#include "graphics/utils/gl_shader.h"
#include "graphics/utils/sdl_graphics.h"
#include "graphics/sprite/AnimationDefines.h"
#include "graphics/Camera.h"
#include "graphics/Text.h"
#include "graphics/render/Atlas.h"
#include "graphics/render/GBuffer.h"
#include "graphics/render/GLHelpers.h"
#include "graphics/render/VBuffer.h"
#include "graphics/render/GLTextures.h"
#include "graphics/utils/VBOArray.h"

#include "scripts/LuaConfig.h"

#include <algorithm>

#include "config.h"

#include "debug.h"
#include "hacks.h"
#include "safestring.h"


extern MainConfig conf;


namespace {

	//////////////////////////////////////////////////
	// Textures

	unsigned int texturesCount;
	TextureInfo* textures;

	//////////////////////////////////////////////////
	// Sprites

	list < Sprite* > sprites_array;

	//std::vector < Sprite* > GLSprites;


	//////////////////////////////////////////////////
	// VBO

	GLuint VBOHandle;

	// Returns count of breakings
	//int VBOHandlesCount;
	//int VBOHandlesSize;
	//VBOStructureHandle* VBOHandles;


	//////////////////////////////////////////////////
	// Atlases

	int atlasWidth, atlasHeight;
	GLuint atlasHandle;
	GLuint normalsHandle;

	void TestDrawAtlas(int x, int y, GLuint atlas)
	{
		if( !atlas )
			atlas = atlasHandle;

		GLHelpers::DrawToQuad( atlas, 0, 0, atlasWidth, atlasHeight );

	#ifdef DEBUG_DRAW_RECTANGLES
		glBegin(GL_LINES);
		for( int i=0; i< 8190; i += 86  ){
			glVertex2f(x + i, y);
			glVertex2f(x + i, y + 300);
		}
		glEnd();
	#endif
	}


	/*
	void update_sprites(  )
	{
		listElement< Sprite* >* it = sprites_array.head;
		while( it != NULL ){
			Sprite* s = it->data;
			s->tex = &textures[s->texid];
			it = it->next;
		}
	}*/
}


void RenderManager::init( )
{
	textures = NULL;
	//verticles = NULL;
	//verticlesSize = 1; // for success multiplication
	atlasHandle = 0;
	// Set first texture info to 0
	textures = (TextureInfo*)malloc( (unsigned)sizeof(TextureInfo) );
	texturesCount = 1;
	memset( &textures[0], 0, (unsigned)sizeof(TextureInfo) );

	std::string n = "0";
	textures[0].id = new char[2];
	strcpy(textures[0].id, n.c_str());

	AnimDef::init();
}


void RenderManager::clean( )
{
	glDeleteBuffers( 1, &VBOHandle );

	TextureAtlas::clean( );
	if( textures ){
		for( unsigned int i = 0; i < texturesCount; ++i )
			delete[] textures[i].id;
		free(textures);
	}
	GLTextures::clearCache();
	CleanFonts();
	VBOArray::clean( );
	GBuffer::clean( );
	ftDone();
}




void RenderManager::openglInit( )
{
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 6 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 6 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 6 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 6 );
}


bool RenderManager::openglSetup( int wwidth, int wheight )
{
	GLExtensions::load();

	// Does not init gbuffer if single mode uses
	if( conf.video.renderMethod == rmGBuffer && !GBuffer::init( ) )
		return false;

	glClear( GL_COLOR_BUFFER_BIT ); // | GL_DEPTH_BUFFER_BIT );

	glTranslatef(0.0f, 0.0f, 6.0f);

	glClearColor( 1, 1, 1, -1 ); //0.25, 0.43, 0.0, -1.0 );

	glClearDepth( 10.0f );

	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	glGenBuffersARB( 1, &VBOHandle );

	TextureAtlas::init( );

	// Create blank texture
	glGenTextures(1, &textures[0].atlas);
	glBindTexture(GL_TEXTURE_2D, textures[0].atlas);
	//GIMP says it is white
	char color[4] = { '\377','\377', '\377', '\377' };
	glTexImage2D( GL_TEXTURE_2D, 0, 4, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, color );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glBindTexture(GL_TEXTURE_2D, 0);

	rect2f view( 0.0, 0.0, conf.video.windowWidth, conf.video.windowHeight );
	Camera::push_state( &view );

	return true;
}


//////////////////////////////////////////////////
// Textures

bool RenderManager::LoadTextures( )
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


int RenderManager::PushTexture( TextureProxy* proxy, GLuint atlas, GLuint normals )
{
	textures = (TextureInfo*)realloc(textures, (unsigned)sizeof(TextureInfo) * ( 1 + texturesCount ) );
	TextureInfo& ti = textures[texturesCount];
	ti.fromTextureProxy( proxy, atlas );
	ti.id = new char[proxy->id.size() + 1];
	ti.normals = normals;
	strcpy(ti.id, proxy->id.c_str());
	//update_sprites();
	return texturesCount++;
}


void RenderManager::PushTextures( std::vector < TextureProxy* >& tarray, GLuint atlas, GLuint normals )
{
	int tcount = tarray.size();
	textures = (TextureInfo*)realloc(textures, (unsigned)sizeof(TextureInfo) * ( tcount + texturesCount ) );
	for( int i = 0; i < tcount; ++i ){
		TextureProxy* proxy = tarray.at( i );
		TextureInfo& ti = textures[texturesCount];
		ti.fromTextureProxy( proxy, atlas );
		ti.id = new char[proxy->id.size() + 1];
		ti.normals = normals;
		strcpy(ti.id, proxy->id.c_str());
		texturesCount++;
	}
	//update_sprites();
}


int RenderManager::GetTextureNumberById( std::string id )
{
	for( unsigned int i = 0; i < texturesCount; ++i ){
		if( id.compare(textures[i].id) == 0 )
			return i;
	}
	return 0;
}


TextureInfo* RenderManager::GetTextureByNumber( unsigned int number )
{
	if( number < texturesCount )
		return &textures[number];
	return NULL;
}



//////////////////////////////////////////////////
// Sprites


/* This function creating sprite structure with texture.
 * x, y, z - right top coordinates;
 * texX, texY - right top coordinates of texture rectangle;
 * width, height - width and height of sprite/texture rectangle;
 * texture_id - texture id;
 * picture - number of picture in texture table;
 * mirrored - rotates the image on the x axis;
 * centered - origin at the center of the object if not 0;
 * returns Sprite*
 */
Sprite* RenderManager::CreateGLSprite( float x, float y, float z, int width, int height,
				unsigned int texture_id, enum primitives shape, short centered )
{
	Sprite* sprite = new Sprite( shape, centered );
	sprite->texid = texture_id;

	if( texture_id >= texturesCount ){
		Debug::debug( Debug::GRAPHICS, "Bad texture id passed.\n" );
		//sprite->tex = NULL;
		//sprite->atlas = 0;
	}else{
		//sprite->tex = &textures[texture_id];
		TextureInfo* tex_info = &textures[texture_id];
		sprite->textures.push_back( tex_info->atlas );
		//sprite->atlas = sprite->tex->atlas;
		sprite->addNormalMap( tex_info->normals );
	}

	sprite->resize( (float)width, (float)height );
	sprite->setPosition( x, y, z );

	sprites_array.push_back( sprite );

	return sprite;
}


// Create new sprite form exists
Sprite* RenderManager::CreateGLSprite( Sprite* osprite )
{
	if( osprite == NULL )
		return NULL;

	Sprite* sprite = new Sprite(osprite);

	sprites_array.push_back( sprite );

	return sprite;
}


void RenderManager::FreeGLSprite( Sprite* sprite )
{
	listElement< Sprite* >* it = sprites_array.head;
	while( it != NULL ){
		if( it->data == sprite )
			break;
		it = it->next;
	}
	sprites_array.remove(it);
	/* if( it == NULL )
		debug( GRAPHICS, "Sprite not under control.\n" ); */
	if( sprite )
		delete sprite;
}


void RenderManager::FreeGLSprites( std::vector< Sprite* >* sprites )
{
	FOREACHPIT(sprites)
		FreeGLSprite(*it);
	sprites->clear();
}


list< Sprite* >* RenderManager::GetSpritesArray( )
{
	return &sprites_array;
}


//////////////////////////////////////////////////
// Scene


inline bool compareSprites( Sprite* s1, Sprite* s2 )
{
	s3f* o1 = &s1->brush.vertex_origin;
	s3f* o2 = &s2->brush.vertex_origin;
	if( o1->z == o2->z ){
		if( o1->y == o2->y ){
			return ( o1->x > o2->x );
		}
		return ( o1->y > o2->y );
	}
	return ( o1->z < o2->z );
}


void RenderManager::DrawGLScene()
{
	Camera::update();

	switch( conf.video.renderMethod ){
		case rmSingle:
		{
			//sort( GLSprites.begin(), GLSprites.end(), compareSprites );
			list< VBOStructureHandle* > vbos;
			VBuffer::prepare_handler( &sprites_array, &vbos );
			VBuffer::setup( VBOHandle );

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			VBuffer::draw( glpDefault, &vbos );
			glDisable(GL_BLEND);
			VBuffer::unbind( );

			VBuffer::free_handler( &vbos );
		}
			break;
		case rmGBuffer:
			GBuffer::render( );
			break;
	}

	//TestDrawAtlas(-2500, -1000, 10);

	SDL_GL_SwapBuffers();
}


void RenderManager::CleanGLScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}
