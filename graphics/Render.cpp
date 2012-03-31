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

#include <algorithm>

#include "debug.h"
#include "hacks.h"

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

	//glEnable( GL_TEXTURE_2D );

	glViewport( 0, 0, wwidth, wheight );

	glClear( GL_COLOR_BUFFER_BIT ); // | GL_DEPTH_BUFFER_BIT );

	glTranslatef(0.0f, 0.0f, 6.0f);

	glClearColor( 0, 0, 0, -1 ); //0.25, 0.43, 0.0, -1.0 );

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

	//glGenBuffers(1, &VBOHandle);
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

	t->texture = LoadGLTexture( t->image );

	internalTextures.push_back(t);

	delete lc;
}


TextureInfo* RenderManager::GetTextureById( std::string id )
{
	for( int i = 0; i < texturesCount; ++i ){
		if( id.compare(textures[i].id) == 0 )
			return &textures[i];
	}
	return NULL;
}


Sprite* RenderManager::CreateGLSprite( float x, float y, float z, int width, int height, int texture_id,
						int picture, short centered )
{
	TextureInfo* tex = NULL;
	if( texture_id > 0){
		if( texture_id >= texturesCount )
			debug( GRAPHICS, "Bad texture id passed.\n" );
		else
			tex = &textures[texture_id];
	}

	return CreateGLSprite( x, y, z, width, height, tex, picture, centered );
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
Sprite* RenderManager::CreateGLSprite( float x, float y, float z, int width, int height, TextureInfo* tex,
						int picture, short centered )
{
	Sprite* sprite = new Sprite();
	sprite->tex = tex;

	if(centered)
		sprite->centered = true;
	sprite->resize( width, height );
	sprite->setPosition( x, y, z );
	sprite->setPicture(picture);

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
	GLuint atlas = BuildAtlas();
	if ( !atlas ){
		debug( GRAPHICS, "Cannot build map.\n" );
		return false;
	}
	atlasHandle = new GLuint(atlas);
	int tcount = internalTextures.size() + texturesCount;
	textures = (TextureInfo*)realloc(textures, sizeof(TextureInfo) * tcount );
	for( int i = texturesCount; i < tcount; ++i ){
		textures[i].fromTextureS(internalTextures[i], atlasHandle);
		textures[i].id = new char[internalTextures[i]->id.size() + 1];
		strcpy(textures[i].id, internalTextures[i]->id.c_str());
		texturesCount++;
	}
	clear_vector( &internalTextures );
	return true;
}


void RenderManager::MoveGlScene( int x, int y, int z )
{
	vpoint.x = x;
	vpoint.y = y;
	vpoint.z = z;
	glTranslatef( x, y, z );
}


void RenderManager::DrawGLScene()
{
	VBOStructureHandle* temp = NULL;
	int count;
	VBOStructureHandle* vbostructure = PrepareVBO(&count);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnable(GL_TEXTURE_2D);
	//glBindBuffer(GL_ARRAY_BUFFER, VBOHandle);

	//glBufferData(GL_ARRAY_BUFFER, sizeof(VertexV2FT2FC4UI) * count, verticles, GL_STREAM_DRAW);

	// Определяем указатели.
	glVertexPointer(3, GL_FLOAT, sizeof(VertexV2FT2FC4UI), &(verticles[0].verticles));
	glTexCoordPointer(2, GL_FLOAT, sizeof(VertexV2FT2FC4UI), &(verticles[0].coordinates));
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(VertexV2FT2FC4UI), &(verticles[0].color));

	while(vbostructure != NULL){
		glBindTexture(GL_TEXTURE_2D, ( vbostructure->texture != NULL ? *(vbostructure->texture->atlas) : 0 ));
		//StartShader(vbostructure->shaders);
		glDrawArrays(GL_QUADS, vbostructure->start, vbostructure->end - vbostructure->start);
		//StopShader(vbostructure->shaders);

		//Clean vbos
		temp = vbostructure;
		vbostructure = vbostructure->next;
		delete temp;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	//TestDrawAtlas(-2500, -1000);

	glLoadIdentity();
	SDL_GL_SwapBuffers();
}


void RenderManager::CleanGLScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}



// PRIVATE


RenderManager::RenderManager( ){
	textures = NULL;
	verticlesSize = 1; // for success multiplication
	minAtlasSize = 64;
	texturesCount = 0;
	GLSprites.clear();
}


RenderManager::~RenderManager( )
{
	clear_vector( &internalTextures );
	free(textures);
	free(verticles);
	ClearGLTexturesCache();
}



Texture* RenderManager::GetGLTexture( string name )
{
	std::map < std::string, Texture* >::iterator it;
	it = texturesCache.find(name);
	if( it != texturesCache.end() ){
		return it->second;
	}
	return NULL;
}

Texture* RenderManager::LoadGLTexture( string name )
{
	Texture* tex;
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

		tex = new Texture();
		tex->tex = SDLGraphics::CreateGlTexture( surface );
		tex->w = surface->w;
		tex->h = surface->h;

		AddGLTexture( name,  tex );

		if( surface )
			SDL_FreeSurface( surface );
	}

	return tex;
}

void RenderManager::AddGLTexture( string name, Texture* texture )
{
	Texture* cached;
	if( !texture )
		return;
	cached = GetGLTexture( name );
	if( cached == texture )
		return;
	FreeGLTexture( cached );
	texturesCache[name] = texture;
}

void RenderManager::FreeGLTexture( Texture* tex )
{
	if( tex && tex->tex ){
		glDeleteTextures( 1, tex->tex );
		delete tex;
		tex = NULL;
	}
}

void RenderManager::ClearGLTexturesCache( )
{
	for( std::map< string, Texture* >::iterator it = texturesCache.begin(), end = texturesCache.end();
			it != end; ++it ){
		FreeGLTexture( it->second );
		it->second = NULL;
	}
	texturesCache.clear();
}




void RenderManager::ExtendVerticles( )
{
	verticlesSize *= 2;
	verticles = (VertexV2FT2FC4UI*)realloc( verticles, sizeof(VertexV2FT2FC4UI) * verticlesSize * 4 );
}


inline bool compareSprites( Sprite* s1, Sprite* s2 )
{
	if( s1->vertices.rt.z == s2->vertices.rt.z ){
		if( s1->posy == s2->posy ){
			return ( s1->posx > s2->posx );
		}
		return ( s1->posy > s2->posy );
	}
	return ( s1->vertices.rt.z < s2->vertices.rt.z );
}

VBOStructureHandle* RenderManager::PrepareVBO(int* c)
{
	sort(GLSprites.begin(), GLSprites.end(), compareSprites);
	int count = 0;
	Sprite* s;
	VBOStructureHandle* v = NULL;
	VBOStructureHandle* first = NULL;
	memset(verticles, '0', verticlesSize);
	for( std::vector< Sprite* >::iterator it = GLSprites.begin(), end = GLSprites.end(); it != end; ++it ){
		s = *(it);
		if( s == NULL || !s->visible )
			continue;
		if( !v ){
			first = v = new VBOStructureHandle(s->tex, 0, count);
		}else if( s->tex != v->texture ){
			v->next = new VBOStructureHandle(s->tex, 0, count);
			v->end = count;
			v = v->next;
		}
		//TODO: fixed in sprites not used
		s3f offset;
		if( s->fixed )
			offset = vpoint;
		verticles[count++] = VertexV2FT2FC4UI(s->vertices.lb - offset, s->coordinates.lt, &s->clr);
		verticles[count++] = VertexV2FT2FC4UI(s->vertices.rb - offset, s->coordinates.rt, &s->clr);
		verticles[count++] = VertexV2FT2FC4UI(s->vertices.rt - offset, s->coordinates.rb, &s->clr);
		verticles[count++] = VertexV2FT2FC4UI(s->vertices.lt - offset, s->coordinates.lb, &s->clr);
	}
	if( v != NULL)
		v->end = count;
	(*c) = count;
	return first;
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


GLuint RenderManager::BuildAtlas()
{
	//atlasHandle = new GLuint();
	GLuint ahandle;

	// Рисуем в атлас, как в текстуру, используя FBO.
	// Настройка текстуры.
	glGenTextures(1, &ahandle);
	glBindTexture(GL_TEXTURE_2D, ahandle);

	glTexImage2D( GL_TEXTURE_2D, 0, 4, atlasWidth, atlasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	//GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)TextureWrapMode.ClampToBorder);
	//GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)TextureWrapMode.ClampToBorder);

	// Настройка FBO.
	GLuint atlasFBO; // Хэндл FBO.

	glGenFramebuffersEXT(1, &atlasFBO);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, atlasFBO);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, ahandle, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if( status != GL_FRAMEBUFFER_COMPLETE_EXT ){
		debug( GRAPHICS, "Your framebuffer is broken. Use top NV to play this Crusis" );
		return 0;
	}

	glPushAttrib(GL_COLOR_BUFFER_BIT); // Сохраняем ClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Изменение вьюпорта и матриц для FBO
	glPushAttrib(GL_VIEWPORT_BIT);
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
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	for ( unsigned int i = 0; i < internalTextures.size(); i++ ){
		glBindTexture(GL_TEXTURE_2D, *(internalTextures[i]->texture->tex));
		glBegin(GL_QUADS);
		{
			float x = static_cast<float>(internalTextures[i]->offsetx) / static_cast<float>(internalTextures[i]->texture->w);
			float y = static_cast<float>(internalTextures[i]->offsety) / static_cast<float>(internalTextures[i]->texture->h);
			float dx = static_cast<float>(internalTextures[i]->width) / static_cast<float>(internalTextures[i]->texture->w);
			float dy = static_cast<float>(internalTextures[i]->height) / static_cast<float>(internalTextures[i]->texture->h);
			//Bottom-left vertex
			glTexCoord2f(x, y);
			glVertex2f(internalTextures[i]->atlasX, internalTextures[i]->atlasY);
			//Bottom-right vertex
			glTexCoord2f(x + dx, y);
			glVertex2f(internalTextures[i]->atlasX + internalTextures[i]->width, internalTextures[i]->atlasY);
			//Top-right vertex
			glTexCoord2f(x + dx, y + dy);
			glVertex2f(internalTextures[i]->atlasX + internalTextures[i]->width, internalTextures[i]->atlasY + internalTextures[i]->height);
			//Top-left vertex
			glTexCoord2f(x, y + dy);
			glVertex2f(internalTextures[i]->atlasX, internalTextures[i]->atlasY + internalTextures[i]->height);
		}
		glEnd();

	}

	// Возвращаем как было.
	glDisable(GL_TEXTURE_2D);

	glDeleteFramebuffersEXT(1, &atlasFBO);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	//viewport
	glPopAttrib();
	//color
	glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	return ahandle;
}



void RenderManager::TestDrawAtlas(int x, int y)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, *atlasHandle);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0, 0.0);
		glVertex2f(x, y);
		glTexCoord2f(1, 0);
		glVertex2f(x + atlasWidth, y);
		glTexCoord2f(1, 1);
		glVertex2f(x + atlasWidth, y + atlasHeight);
		glTexCoord2f(0, 1);
		glVertex2f(x, y + atlasHeight);
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

