/*
 * Render.cpp
 *
 *  Created on: 19.03.2012
 */
#define GL_GLEXT_PROTOTYPES

#include "Render.h"
#include "gl_extensions.h"
#include "sdl_graphics.h"
#include "ElasticBox.h"
#include "Luaconfig.h"

#include "debug.h"

using std::string;
using namespace Debug;


RenderManager* RenderManager::graph = NULL;


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

	glEnable( GL_TEXTURE_2D );

	glViewport( 0, 0, wwidth, wheight );

	glClear( GL_COLOR_BUFFER_BIT ); // | GL_DEPTH_BUFFER_BIT );

	glTranslatef(0.0f, 0.0f, 6.0f);

	glClearColor( 0.25, 0.43, 0.0, -1.0 );

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

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxAtlasSize);

	glGenBuffers(1, &VBOHandle);
}


bool RenderManager::LoadTextures( )
{
	LuaConfig* lc = new LuaConfig;
	std::vector< string > names;
	int textures_count;
	string config = "sprite";

	lc->getSubconfigsLength(config, textures_count);

	char dbg[20];
	snprintf( dbg, 20, "%d textures found.\n", textures_count );
	debug( GRAPHICS, dbg );

	if( !textures_count )
		return false;

	lc->getSubconfigsList(config, names);

	for( std::vector< string >:: iterator it = names.begin(), vend = names.end(); it != vend; ++it ){
		AddTexture(*it);
	}
	return CreateAtlas();
}


void RenderManager::AddTexture( string name ){
	LuaConfig* lc = new LuaConfig;
	string config = "sprite";
	TextureS* t = new TextureS();

	lc->getValue("id", name, config, t->id);
	lc->getValue("image", name, config, t->image);
	lc->getValue("width", name, config, t->width);
	lc->getValue("height", name, config, t->height);
	lc->getValue("rows", name, config, t->rows);
	lc->getValue("columns", name, config, t->cols);
	lc->getValue("offsetx", name, config, t->offsetx);
	lc->getValue("offsety", name, config, t->offsety);

	t->texture = LoadGLTexture( name );

	internalTextures.push_back(t);
}


void RenderManager::AddTexture( string id, string name, string path, int width, int height,
							int offsetx, int offsety, int rows, int cols)
{
	TextureS* t = new TextureS(id, name, path, width, height, offsetx, offsety, rows, cols);
	t->texture = LoadGLTexture( path );
	internalTextures.push_back(t);
}


GLuint* RenderManager::GetGLTexture( string name )
{
	std::map < string, Texture* >::iterator it;
	it = texturesCache.find(name);
	if( it != texturesCache.end() ){
		return it->second;
	}
	return NULL;
}


GLuint* RenderManager::LoadGLTexture( string name )
{
	GLuint* tex;
	SDL_Surface* surface;

	if( name == "" )
		return NULL;

	tex = GetGLTexture( name );

	if( !tex ){

		surface = SDLGraphics::LoadImage( name.c_str() );

		if( !surface ){
			debug( GRAPHICS, name + " not loaded.\n" );
			return NULL;
		}

		tex = SDLGraphics::CreateGlTexture( surface );

		AddGLTexture( name,  tex );

		if( surface )
			SDL_FreeSurface( surface );
	}

	return tex;
}


void RenderManager::AddGLTexture( string name, GLuint* texture )
{
	GLuint* cached;
	if( !texture )
		return;
	cached = GetGLTexture( name );
	if( cached == texture )
		return;
	FreeGLTexture( cached );
	texturesCache[name] = texture;
}


/* This function creating sprite structure with texture.
 * x, y, z - right top coordinates;
 * texX, texY - right top coordinates of texture rectangle;
 * width, height - width and height of sprite/texture rectangle;
 * texture_id - texture id;
 * picture - number of picture in texture table;
 * mirrored - rotates the image on the x axis;
 * centered - origin at the center of the object if not 0;
 * return Sprite*
 */
Sprite* RenderManager::CreateGLSprite( float x, float y, float z, int width, int height, int texture_id,
						int picture, short centered = 0 )
{
	Sprite* sprite = new Sprite();
	sprite->vertices = new vertex3farr();
	sprite->clr = new color4u();

	if(centered)
		sprite->centered = true;
	sprite->resize( width, height );
	sprite->setPosition( x, y, z );

	GLSprites.push_back( sprite );

	ExtendVerticles( GLSprites.size() - verticlesSize );

	return sprite;
}

void RenderManager::FreeGLSprite( Sprite* sprite )
{
	std::vector< Sprite* >::iterator it;
	it = std::find( GLSprites.begin(), GLSprites.end(), sprite );
	if( it != GLSprites.end() )
		GLSprites.erase(it);
	delete sprite->vertices;

}


bool RenderManager::CreateAtlas( )
{
	if( internalTextures.size() < 1 ){
		debug( GRAPHICS, "Textures is missing" );
		return false;
	}
	sort( internalTextures.begin(), internalTextures.end() );
	if( !BuildAtlasMap() ){
		debug( GRAPHICS, "Cannot build atlas map.\n" );
		return false;
	}
	if ( !BuildAtlas() ){
		debug( GRAPHICS, "Cannot build map.\n" );
		return false;
	}
	textures = (TextureInfo)malloc( sizeof(TextureInfo) * internalTextures.size() );
	for( unsigned int i = 0; i < internalTextures.size(); ++i ){
		textures[i].fromTextureS(internalTextures[i], atlasHandle);
	}
	clear_vector( &internalTextures );
	return true;
}


void RenderManager::MoveGlScene( int x, int y, int z )
{
	//FIXME: double cast
	vpoint.x = x;
	vpoint.y = y;
	vpoint.z = z;
	glTranslatef( x, y, z );
}


void RenderManager::DrawGLScene()
{
	VBOStructureHandle* vbostructure = NULL;
	VBOStructureHandle* temp = NULL;
	int count = PrepareVBO(vbostructure);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glBindBuffer(GL_ARRAY_BUFFER, VBOHandle);

	// Определяем указатели.
	glVertexPointer(2, GL_FLOAT, sizeof(VertexV2FT2FC4UI), 0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(VertexV2FT2FC4UI), (const GLubyte*)0 + sizeof(s2f));
	glColorPointer(4, GL_INT, sizeof(VertexV2FT2FC4UI), (const GLubyte*)0 + (sizeof(s2f) * 2));

	while(vbostructure != NULL){
		glBindTexture(GL_TEXTURE_2D, *(vbostructure->texture->atlas));
		//StartShader(vbostructure->shaders);
		glDrawArrays(GL_QUADS, vbostructure->number,
					((vbostructure->next != NULL ? vbostructure->next->number : count) - vbostructure->number) * 4);
		//StopShader(vbostructure->shaders);
		glBindTexture(GL_TEXTURE_2D, 0);

		//Clean vbos
		temp = vbostructure;
		vbostructure = vbostructure->next;
		delete temp;
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glLoadIdentity();
	SDL_GL_SwapBuffers();
}


void RenderManager::CleanGLScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}



// PRIVATE


RenderManager::RenderManager( ){
	VBOHandle = NULL;
	verticlesSize = 0;
	minAtlasSize = 64;
}


RenderManager::~RenderManager( )
{
	clear_vector( &internalTextures );
	free(textures);
	free(verticles);
	ClearGLTexturesCache();
}



void RenderManager::FreeGLTexture( GLuint* tex )
{
	if( tex ){
		glDeleteTextures( 1, tex );
		tex = NULL;
	}
}

void RenderManager::ClearGLTexturesCache( )
{
	for( std::map< string, GLuint* >::iterator it = texturesCache.begin(), end = texturesCache.end();
			it != end; ++it ){
		FreeGLTexture( it->second );
		it->second = NULL;
	}
	texturesCache.clear();
}


void RenderManager::ExtendVerticles(int count)
{
	if( count <= 0 )
		return;
	verticlesSize += count;
	verticles = (VertexV2FT2FC4UI)realloc( verticles, sizeof(VertexV2FT2FC4UI) * verticlesSize );
}


int RenderManager::PrepareVBO(VBOStructureHandle* v)
{
	sort(GLSprites.begin(), GLSprites.end());
	int count = 0;
	Sprite* s;
	for(int sprite=GLSprites.size(); sprite >= 0 ; --sprite){
		s = GLSprites[sprite];
		if(!s->visible)
			continue;
		if(!v || s->tex != v->texture)
			v = new VBOStructureHandle(s->tex, 0, sprite, v);
		coord2farr texcoord = s->getTextureCoordinates();
		verticles[sprite    ] = VertexV2FT2FC4UI(s->vertices->lt, texcoord.lt, s->clr);
		verticles[sprite + 1] = VertexV2FT2FC4UI(s->vertices->rt, texcoord.rt, s->clr);
		verticles[sprite + 2] = VertexV2FT2FC4UI(s->vertices->rb, texcoord.rb, s->clr);
		verticles[sprite + 3] = VertexV2FT2FC4UI(s->vertices->lb, texcoord.lb, s->clr);
		count++;
	}
	return count;
}



bool RenderManager::BuildAtlasMap()
{
	float texelW = 1.0f;
	float texelH = 1.0f;

	ElasticBox box = ElasticBox(minAtlasSize, maxAtlasSize);

	for( std::vector < TextureS* >:: iterator it = internalTextures.begin(),
			vend = internalTextures.end(); it != vend; ++it ){
		if ( ! box.InsertItem(
				&((*it)->atlasX), &((*it)->atlasY),
				(*it)->width, (*it)->height ) )
			return false;
	}

	this->atlasWidth = box.Width;
	this->atlasHeight = box.Height;

	texelW = texelW / static_cast<float>( box.Width );
	texelH = texelH / static_cast<float>( box.Height );

	// Build absolute map
	for( unsigned int i = 0; i < internalTextures.size(); i++ ){
		internalTextures[i]->atlas.x = static_cast<float>(internalTextures[i]->atlasX) * texelW;
		internalTextures[i]->atlas.y = static_cast<float>(internalTextures[i]->atlasY) * texelH;
		internalTextures[i]->atlas.width = internalTextures[i]->width * texelW;
		internalTextures[i]->atlas.height = internalTextures[i]->height * texelH;
	}

	return true;
}


bool RenderManager::BuildAtlas()
{
	atlasHandle = new GLuint();

	// Рисуем в атлас, как в текстуру, используя FBO.
	// Настройка текстуры.
	glGenTextures(1, atlasHandle);
	glBindTexture(GL_TEXTURE_2D, *atlasHandle);

	glTexImage2D( GL_TEXTURE_2D, 0, 4, atlasWidth, atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	//GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)TextureWrapMode.ClampToBorder);
	//GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)TextureWrapMode.ClampToBorder);

	// Настройка FBO.
	GLuint atlasFBO; // Хэндл FBO.

	glGenFramebuffersEXT(1, &atlasFBO);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, atlasFBO);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, *atlasHandle, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if( status != GL_FRAMEBUFFER_COMPLETE_EXT ){
		debug( GRAPHICS, "Your framebuffer is broken. Use top NV to play this Crusis" );
		return false;
	}

	glPushAttrib(GL_COLOR_BUFFER_BIT); // Сохраняем ClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Изменение вьюпорта и матриц для FBO
	glViewport(0, 0, atlasWidth, atlasHeight);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, atlasWidth, 0, atlasHeight, -2.0, 2.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Отрисовка текстур в атлас.
	glClear(GL_COLOR_BUFFER_BIT);
	glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_TEXTURE_2D);
	for ( unsigned int i = 0; i < internalTextures.size(); i++ ){
		glBindTexture(GL_TEXTURE_2D, *internalTextures[i]->texture);
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0.0, 0.0); glVertex2f(internalTextures[i]->atlasX, internalTextures[i]->atlasY);
			glTexCoord2f(1.0, 0.0); glVertex2f(internalTextures[i]->atlasX + internalTextures[i]->width, internalTextures[i]->atlasY);
			glTexCoord2f(1.0, 1.0); glVertex2f(internalTextures[i]->atlasX + internalTextures[i]->width, internalTextures[i]->atlasY + internalTextures[i]->height);
			glTexCoord2f(0.0, 1.0); glVertex2f(internalTextures[i]->atlasX, internalTextures[i]->atlasY + internalTextures[i]->height);
		}
		glEnd();
	}

	// Возвращаем как было.
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT(1, &atlasFBO);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glPopAttrib();

	return true;
}

