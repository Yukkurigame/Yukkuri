/*
 * Render.cpp
 *
 *  Created on: 19.03.2012
 */

#include "Render.h"
#include "graphics/gl_extensions.h"
#include "graphics/utils/gl_shader.h"
#include "graphics/sprite/AnimationDefines.h"
#include "graphics/sprite/Mesh.h"
#include "graphics/Camera.h"
#include "graphics/Text.h"
#include "graphics/render/Atlas.h"
#include "graphics/render/GBuffer.h"
#include "graphics/render/GLHelpers.h"
#include "graphics/render/Textures.h"
#include "graphics/render/VBuffer.h"
#include "graphics/render/GLTextures.h"
#include "graphics/utils/VBOArray.h"
#include "graphics/Lighting.h"

#include "SDL/SDL.h"

#include "config.h"

#include "debug.h"
#include "hacks.h"
#include "safestring.h"


extern MainConfig conf;


namespace {

	//////////////////////////////////////////////////
	// Sprites

	list < Sprite* > sprites_array;

	//////////////////////////////////////////////////
	// VBO

	GLuint VBOHandle;

	//////////////////////////////////////////////////
	// Atlases

	int atlasWidth, atlasHeight;
	//GLuint atlasHandle;
	//GLuint normalsHandle;

	void TestDrawAtlas(int x, int y, GLuint atlas)
	{
		//if( !atlas )
		//	atlas = atlasHandle;

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

}


void RenderManager::init( )
{
	Textures::init();
	AnimDef::init();
	MeshManager::init();
}


void RenderManager::clean( )
{
	glDeleteBuffers( 1, &VBOHandle );

	TextureAtlas::clean( );
	Textures::clean( );
	GLTextures::clean();
	clean_fonts();
	VBOArray::clean( );
	GBuffer::clean( );
	MeshManager::clean();
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

	//glTranslatef(0.0f, 0.0f, 6.0f);

	glClearColor( 1, 1, 1, -1 ); //0.25, 0.43, 0.0, -1.0 );

	//glClearDepth( 600.0f );

	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	glGenBuffersARB( 1, &VBOHandle );

	TextureAtlas::init( );

	char color[4] = { '\377','\377', '\377', '\377' };

	GLuint nt = 0;
	TextureProxy first;
	GLTextures::generate( &nt, 1, 1, GL_UNSIGNED_BYTE, color );
	first.texture = GLTextures::create( "", nt, 1, 1 );
	Textures::push( &first, nt, 0 );

	rect2f view( 0.0, 0.0, conf.video.windowWidth, conf.video.windowHeight );
	s2f z( -conf.video.windowHeight * 2, conf.video.windowHeight );

	Camera::init();

	Camera::push_state( &view, &z );
	Camera::Rotate( 55, 1.0, 0.0, 0.0 );
	Camera::Rotate( 45, 0.0, 0.0, 1.0 );
	//Camera::Translate( -conf.video.windowWidth/4, 0, 0 );

	LightingManager::init( );

	return true;
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

	TextureInfo* tex_info = Textures::get_pointer( texture_id );
	if( !tex_info ){
		Debug::debug( Debug::GRAPHICS, "Bad texture id passed.\n" );
	}else{
		sprite->textures.push_back( tex_info->atlas );
		sprite->addNormalMap( tex_info->normals );
	}

	sprite->resize( (float)width, (float)width, (float)height );
	sprite->setPosition( x, y, z );

	sprites_array.push( sprite );

	return sprite;
}


// Create new sprite form exists
Sprite* RenderManager::CreateGLSprite( Sprite* osprite )
{
	if( osprite == NULL )
		return NULL;

	Sprite* sprite = new Sprite(osprite);

	sprites_array.push( sprite );

	return sprite;
}


void RenderManager::FreeGLSprite( Sprite* sprite )
{
	listElement< Sprite* >* it = sprites_array.find( sprite );
	sprites_array.remove(it);
	/* if( it == NULL )
		debug( GRAPHICS, "Sprite not under control.\n" ); */
	if( sprite )
		delete sprite;
}


void RenderManager::FreeGLSprites( list< Sprite* >* sprites )
{
	ITER_LISTP( Sprite*, sprites )
		FreeGLSprite(it->data);
	sprites->clear();
}


list< Sprite* >* RenderManager::GetSpritesArray( )
{
	return &sprites_array;
}


//////////////////////////////////////////////////
// Scene

void RenderManager::DrawGLScene()
{
	Camera::update();

	sprites_array.sort( compareSprites );

	switch( conf.video.renderMethod ){
		case rmSingle:
		{
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
