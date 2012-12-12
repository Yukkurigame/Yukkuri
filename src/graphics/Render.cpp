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
#include "graphics/render/GLHelpers.h"
#include "graphics/render/GLTextures.h"
#include "graphics/render/TextureArray.h"

#include "scripts/LuaConfig.h"

#include <algorithm>

#include "config.h"

#include "debug.h"
#include "hacks.h"
#include "safestring.h"


namespace {

	//////////////////////////////////////////////////
	// Textures

	unsigned int texturesCount;
	TextureInfo* textures;

	//////////////////////////////////////////////////
	// Sprites

	std::vector < Sprite* > GLSprites;


	//////////////////////////////////////////////////
	// Verticles

	int verticlesSize;
	VertexV2FT2FC4UI* verticles;

	void ExtendVerticles( )
	{
		verticlesSize *= 2;
		verticles = (VertexV2FT2FC4UI*)realloc( verticles, (unsigned)sizeof(VertexV2FT2FC4UI) * verticlesSize * 4 );
	}


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


}



void RenderManager::init( )
{
	textures = NULL;
	verticles = NULL;
	verticlesSize = 1; // for success multiplication
	atlasHandle = 0;
	GLSprites.clear();
	// Set first texture info to 0
	textures = (TextureInfo*)malloc( (unsigned)sizeof(TextureInfo) );
	texturesCount = 1;
	memset( &textures[0], 0, (unsigned)sizeof(TextureInfo) );

	std::string n = "0";
	textures[0].id = new char[2];
	strcpy(textures[0].id, n.c_str());

	AnimDef::init();
	Camera::init( &vpoint );

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
	if( verticles )
		free(verticles);
	GLTextures::clearCache();
	CleanFonts();
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


void RenderManager::openglSetup( int wwidth, int wheight )
{
	GLExtensions::load();

	//glEnable( GL_TEXTURE_2D );

	glViewport( 0, 0, wwidth, wheight );

	glClear( GL_COLOR_BUFFER_BIT ); // | GL_DEPTH_BUFFER_BIT );

	glTranslatef(0.0f, 0.0f, 6.0f);

	glClearColor( 1, 1, 1, -1 ); //0.25, 0.43, 0.0, -1.0 );

	glClearDepth( 10.0f );

	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LEQUAL);

	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	//glEnable(GL_ALPHA_TEST); //It's work but with ugly border
	//glAlphaFunc(GL_NOTEQUAL, 0);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glOrtho(0.0, wwidth, 0.0, wheight, -10.0, 1.0);
	//glOrtho(-wwidth*1.5, wwidth*1.5, -wheight*1.5, wheight*1.5, -10.0, 1.0);
	//glOrtho(-wwidth*2, wwidth*2, -wheight*2, wheight*2, -30.0, 30.0);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

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
	return TextureAtlas::create( &atlasHandle, atlasWidth, atlasHeight );
}


int RenderManager::PushTexture( TextureProxy* proxy, GLuint atlas )
{
	textures = (TextureInfo*)realloc(textures, (unsigned)sizeof(TextureInfo) * ( 1 + texturesCount ) );
	textures[texturesCount].fromTextureProxy( proxy, atlas );
	textures[texturesCount].id = new char[proxy->id.size() + 1];
	strcpy(textures[texturesCount].id, proxy->id.c_str());
	// Update sprites
	FOREACHIT( GLSprites )
		(*it)->tex = &textures[(*it)->texid];
	return texturesCount++;
}

void RenderManager::PushTextures( std::vector < TextureProxy* >& tarray, GLuint atlas )
{
	int tcount = tarray.size();
	textures = (TextureInfo*)realloc(textures, (unsigned)sizeof(TextureInfo) * ( tcount + texturesCount ) );
	for( int i = 0; i < tcount; ++i ){
		TextureProxy* proxy = tarray.at( i );
		textures[texturesCount].fromTextureProxy( proxy, atlas );
		textures[texturesCount].id = new char[proxy->id.size() + 1];
		strcpy(textures[texturesCount].id, proxy->id.c_str());
		texturesCount++;
	}
	// Update sprites
	FOREACHIT( GLSprites )
		(*it)->tex = &textures[(*it)->texid];
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
				unsigned int texture_id, int picture, short centered )
{
	Sprite* sprite = new Sprite();
	sprite->texid = texture_id;

	if( texture_id < 0 || texture_id >= texturesCount ){
		Debug::debug( Debug::GRAPHICS, "Bad texture id passed.\n" );
		sprite->tex = NULL;
		sprite->atlas = 0;

	}else{
		sprite->tex = &textures[texture_id];
		sprite->atlas = sprite->tex->atlas;
	}

	if(centered)
		sprite->setCentered();
	sprite->resize( (float)width, (float)height );
	sprite->setPosition( x, y, z );
	sprite->setPicture(picture);

	GLSprites.push_back( sprite );

	int vcount = GLSprites.size() - verticlesSize;
	if( vcount > 0 )
		ExtendVerticles( );

	return sprite;
}


// Create new sprite form exists
Sprite* RenderManager::CreateGLSprite( Sprite* osprite )
{
	if( osprite == NULL )
		return NULL;

	Sprite* sprite = new Sprite(osprite);
	/*sprite->texid = osprite->texid;
	sprite->atlas = osprite->atlas;
	sprite->tex = osprite->tex;

	sprite->flags = osprite->flags;
	sprite->resize( osprite->rect.width, osprite->rect.height );
	sprite->setPosition( osprite->rect.x, osprite->rect.y, osprite->vertices.lb.z );
	sprite->setPicture( osprite->picture);
	*/

	GLSprites.push_back( sprite );

	int vcount = GLSprites.size() - verticlesSize;
	if( vcount > 0 )
		ExtendVerticles( );

	return sprite;
}


void RenderManager::FreeGLSprite( Sprite* sprite )
{
	std::vector< Sprite* >::iterator it;
	it = std::find( GLSprites.begin(), GLSprites.end(), sprite );
	if( it != GLSprites.end() ){
		GLSprites.erase(it);
	}else{
		//debug( GRAPHICS, "Sprite not under control.\n" );
	}
	if( sprite )
		delete sprite;
}


void RenderManager::FreeGLSprites( std::vector< Sprite* >* sprites )
{
	FOREACHPIT(sprites)
		FreeGLSprite(*it);
	sprites->clear();
}


//////////////////////////////////////////////////
// Scene

static const std::string fixedShaders[] = {
	"fixed", "daytime", ""
};


void RenderManager::MoveGlScene( )
{
	glTranslatef( vpoint.x, vpoint.y, vpoint.z );
	int i = 0;
	std::string fname = fixedShaders[i++];

	while( fname != "" ){
		GLuint fixed = Shaders::getProgram( fname );
		glUseProgram( fixed );
		glUniform3fv( glGetUniformLocation( fixed, "offset" ), 1, &vpoint.x );
		fname = fixedShaders[i++];
	}
	glUseProgram( 0 );
}

void RenderManager::DrawGLScene()
{
	Camera::Update();
	MoveGlScene();

	//VBOStructureHandle* temp = NULL;
	int count = 0;
	VBOStructureHandle* vbostructure = TextureArray::prepareVBO( /*&count,*/ GLSprites /*, verticles*/ );

	GLHelpers::DrawVBO( VBOHandle, /*count, */ vbostructure /*, verticles */ );

	//TestDrawAtlas(-2500, -1000, 10);

	glLoadIdentity();
	SDL_GL_SwapBuffers();
}


void RenderManager::CleanGLScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}
