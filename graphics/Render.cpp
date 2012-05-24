/*
 * Render.cpp
 *
 *  Created on: 19.03.2012
 */

#include "Define.h"

#include "Render.h"
#include "graphics/gl_extensions.h"
#include "graphics/gl_shader.h"
#include "graphics/sdl_graphics.h"
#include "graphics/Camera.h"
#include "ElasticBox.h"
#include "LuaConfig.h"


#include <algorithm>

#include "config.h"

#include "debug.h"
#include "hacks.h"
#include "safestring.h"

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

	debug( GRAPHICS, citoa(textures_count) + " textures found.\n" );

	if( !textures_count ){
		delete lc;
		return false;
	}

	lc->getSubconfigsList(config, names);

	for( std::vector< string >:: iterator it = names.begin(), vend = names.end(); it != vend; ++it ){
		AddTexture(*it);
	}

	delete lc;
	// load basic textures to main atlas
	return CreateAtlas( &atlasHandle, &atlasWidth, &atlasHeight );
}


void RenderManager::AddTexture( string name ){
	LuaConfig* lc = new LuaConfig;
	string config = "sprite";
	TextureProxy* t = new TextureProxy();

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

void RenderManager::AddTexture( string id, Texture* tex, int width, int height, int cols, int rows, int ax, int ay ){
	TextureProxy* t = new TextureProxy();
	t->id = id;
	t->width = width;
	t->height = height;
	t->cols = (cols ? cols : 1);
	t->rows = (rows ? rows : 1);
	t->offsetx = t->offsety = 0;
	t->atlasX = ax;
	t->atlasY = ay;
	t->texture = tex;
	internalTextures.push_back(t);
}

int RenderManager::GetTextureNumberById( std::string id )
{
	for( int i = 0; i < texturesCount; ++i ){
		if( id.compare(textures[i].id) == 0 )
			return i;
	}
	return 0;
}


/* This function render textures array into opengl texture.
 * ahandle - pointer on opengl texture, if points to 0 new texture will be generated;
 * width, height - width and height of new texture;
 * textures - array of textures coordinates to draw;
 * returns boolean
 */
bool RenderManager::DrawToGLTexture( GLuint* ahandle, int width, int height, std::vector< TextureProxy* >* textures )
{
	// Рисуем в атлас, как в текстуру, используя FBO.
	// Настройка текстуры.
	if( ahandle == NULL ){
		debug( GRAPHICS, "Bad texture for atlas.\n" );
		return false;
	}
	int target = GL_PROXY_TEXTURE_2D;
	if( *ahandle == 0 ){
		glGenTextures(1, ahandle);
		target = GL_TEXTURE_2D;
	}
	glBindTexture(GL_TEXTURE_2D, *ahandle);
	glTexImage2D( target, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	//GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapS, (int)TextureWrapMode.ClampToBorder);
	//GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureWrapT, (int)TextureWrapMode.ClampToBorder);

	GLuint FBOHandle;

	glGenFramebuffersEXT(1, &FBOHandle);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FBOHandle);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, *ahandle, 0);

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if( status != GL_FRAMEBUFFER_COMPLETE_EXT ){
		debug( GRAPHICS, "Your framebuffer is broken. Use top NV to play this Crusis.\n" );
		return 0;
	}

	glPushAttrib(GL_COLOR_BUFFER_BIT); // Сохраняем ClearColor
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Изменение вьюпорта и матриц для FBO
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, width, 0, height, -2.0, 2.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// Отрисовка текстур в атлас.
	glClear(GL_COLOR_BUFFER_BIT);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	for ( unsigned int i = 0; i < textures->size(); i++ ){
		TextureProxy* t = textures->at(i);
		glBindTexture(GL_TEXTURE_2D, t->texture->tex);
		glBegin(GL_QUADS);
		{
			float x = static_cast<float>(t->offsetx) / static_cast<float>(t->texture->w);
			float y = static_cast<float>(t->offsety) / static_cast<float>(t->texture->h);
			float dx = static_cast<float>(t->width) / static_cast<float>(t->texture->w);
			float dy = static_cast<float>(t->height) / static_cast<float>(t->texture->h);
			//Bottom-left vertex
			glTexCoord2f(x, y);
			glVertex2f(t->atlasX, t->atlasY);
			//Bottom-right vertex
			glTexCoord2f(x + dx, y);
			glVertex2f(t->atlasX + t->width, t->atlasY);
			//Top-right vertex
			glTexCoord2f(x + dx, y + dy);
			glVertex2f(t->atlasX + t->width, t->atlasY + t->height);
			//Top-left vertex
			glTexCoord2f(x, y + dy);
			glVertex2f(t->atlasX, t->atlasY + t->height);
		}
		glEnd();

#ifdef DEBUG_DRAW_ATLAS_RECTANGLES
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_LINE_LOOP);
			glVertex2f(t->atlasX, t->atlasY);
			glVertex2f(t->atlasX + t->width, t->atlasY);
			glVertex2f(t->atlasX + t->width, t->atlasY + t->height);
			glVertex2f(t->atlasX, t->atlasY + t->height);
		glEnd();
		glEnable(GL_TEXTURE_2D);
#endif

	}

	// Возвращаем как было.
	glDisable(GL_TEXTURE_2D);

	glDeleteFramebuffersEXT(1, &FBOHandle);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	//viewport
	glPopAttrib();
	//color
	// Everything said that it is unnecessary
	//glPopAttrib();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	return true;
}



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
Sprite* RenderManager::CreateGLSprite( float x, float y, float z, int width, int height, int texture_id,
						int picture, short centered )
{
	Sprite* sprite = new Sprite();
	sprite->texid = texture_id;

	if( texture_id < 0 || texture_id >= texturesCount ){
		debug( GRAPHICS, "Bad texture id passed.\n" );
		texture_id = 0;
		sprite->tex = NULL;
	}else{
		sprite->tex = &textures[texture_id];
	}

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

// Create new sprite form exists
Sprite* RenderManager::CreateGLSprite( Sprite* osprite )
{
	if( osprite == NULL )
		return NULL;

	Sprite* sprite = new Sprite();
	sprite->texid = osprite->texid;
	sprite->tex = osprite->tex;


	sprite->centered = osprite->centered;
	sprite->resize( osprite->width, osprite->height );
	sprite->setPosition( osprite->posx, osprite->posy, osprite->vertices.lb.z );
	sprite->setPicture( osprite->picture);

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
	for( std::vector< Sprite* >:: iterator it = sprites->begin(), vend = sprites->end(); it != vend; ++it ){
		FreeGLSprite(*it);
	}
	sprites->clear();
}


void RenderManager::AddShader( Sprite* sprite, std::string name )
{
	if( sprite == NULL )
		return;
	sprite->shader = CreateProgram( name );
}



inline bool compareTextureProxies( TextureProxy* t1, TextureProxy* t2 )
{
	return t2->width * t2->height - t1->width * t1->height;
}

bool RenderManager::CreateAtlas( GLuint* ahandle, int* width, int* height, short map )
{
	if( internalTextures.size() < 1 ){
		debug( GRAPHICS, "Textures is missing.\n" );
		return false;
	}
	//FIXME: sorting probably not works.
	sort( internalTextures.begin(), internalTextures.end(), compareTextureProxies );
	if( !BuildAtlasMap( width, height ) ){
		debug( GRAPHICS, "Cannot build atlas map.\n" );
		return false;
	}
	if ( !BuildAtlas( ahandle, *width, *height ) ){
		debug( GRAPHICS, "Cannot draw atlas.\n" );
		return false;
	}
	int tcount = internalTextures.size();
	textures = (TextureInfo*)realloc(textures, sizeof(TextureInfo) * ( tcount + texturesCount ) );
	for( int i = 0; i < tcount; ++i ){
		textures[texturesCount].fromTextureProxy(internalTextures[i], *ahandle);
		textures[texturesCount].id = new char[internalTextures[i]->id.size() + 1];
		strcpy(textures[texturesCount].id, internalTextures[i]->id.c_str());
		texturesCount++;
	}
	for( std::vector< Sprite* >::iterator it = GLSprites.begin(), end = GLSprites.end(); it != end; ++it )
		(*it)->tex = &textures[(*it)->texid];
	clear_vector( &internalTextures );
	return true;
}


void RenderManager::MoveGlScene( )
{
	glTranslatef( vpoint.x, vpoint.y, vpoint.z );
}

void RenderManager::DrawGLScene()
{
	Camera::Update();
	MoveGlScene();

	VBOStructureHandle* temp = NULL;
	int count;
	VBOStructureHandle* vbostructure = PrepareVBO(&count);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glBindBuffer(GL_ARRAY_BUFFER, VBOHandle);

	// VBO + GL_STREAM_DRAW == +10 fps
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexV2FT2FC4UI) * count, verticles, GL_STREAM_DRAW);

	// Определяем указатели.
	//glVertexPointer(3, GL_FLOAT, sizeof(VertexV2FT2FC4UI), &(verticles[0].verticles));
	//glTexCoordPointer(2, GL_FLOAT, sizeof(VertexV2FT2FC4UI), &(verticles[0].coordinates));
	//glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(VertexV2FT2FC4UI), &(verticles[0].color));
	glVertexPointer(3, GL_FLOAT, sizeof(VertexV2FT2FC4UI), 0);
	glTexCoordPointer(2, GL_FLOAT, sizeof(VertexV2FT2FC4UI), (void*)0 + sizeof(s3f));
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(VertexV2FT2FC4UI), (void*)0 + sizeof(s3f) + sizeof(s2f));

	//for( int i=0; i < VBOHandlesCount; i++ ){
	while(vbostructure != NULL){
		//glBindTexture(GL_TEXTURE_2D, textures[VBOHandles[i].texture].atlas);
		glBindTexture(GL_TEXTURE_2D, textures[vbostructure->texture].atlas);
		//StartShader(vbostructure->shaders);
		//glDrawArrays(GL_QUADS, VBOHandles[i].start, VBOHandles[i].count);
	    glUseProgram( vbostructure->shader );
		glDrawArrays(GL_QUADS, vbostructure->start, vbostructure->count);
		//StopShader(vbostructure->shaders);

		//Clean vbos
		temp = vbostructure;
		vbostructure = vbostructure->next;
		delete temp;
	}
	glBindTexture(GL_TEXTURE_2D, 0);

#ifdef DEBUG_DRAW_RECTANGLES
	for( int i = 0; i < count; i = i + 4 )
		glDrawArrays(GL_LINE_LOOP, i, 4);
#endif

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	//TestDrawAtlas(-2500, -1000, 10);

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
	verticles = NULL;
	verticlesSize = 1; // for success multiplication
	minAtlasSize = 64;
	atlasHandle = 0;
	GLSprites.clear();
	// Set first texture info to 0
	textures = (TextureInfo*)malloc( sizeof(TextureInfo) );
	texturesCount = 1;
	memset( &textures[0], 0, sizeof(TextureInfo) );

	std::string n = "0";
	textures[0].id = new char[2];
	strcpy(textures[0].id, n.c_str());

	Camera::init( &vpoint );
}


RenderManager::~RenderManager( )
{
	clear_vector( &internalTextures );
	if( textures ){
		for( int i = 0; i < texturesCount; ++i )
			delete[] textures[i].id;
		free(textures);
	}
	if( verticles )
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
		glDeleteTextures( 1, &tex->tex );
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

VBOStructureHandle* RenderManager::PrepareVBO( int* c )
{
	sort(GLSprites.begin(), GLSprites.end(), compareSprites);
	int count = 0;
	Sprite* s;
	//VBOHandlesCount = 0;
	VBOStructureHandle* v = NULL;
	VBOStructureHandle* first = NULL;
	//int lastText = -1;
	//memset( verticles, '0', sizeof( VertexV2FT2FC4UI ) * verticlesSize );
#ifdef RENDER_VISIBLE
	extern MainConfig conf;
#endif
	for( std::vector< Sprite* >::iterator it = GLSprites.begin(), end = GLSprites.end(); it != end; ++it ){
		s = *(it);
		if( s == NULL || !s->visible )
			continue;
#ifdef RENDER_VISIBLE
		vertex3farr& pos = s->vertices;
		if( !s->fixed && ( pos.lb.x + vpoint.x < -100 || pos.rb.x + vpoint.x > conf.windowWidth + 100 ||
			pos.lb.y + vpoint.y < -100 || pos.lt.y + vpoint.y > conf.windowHeight + 100 ) )
			continue;
#endif
		/*if( s->texid != lastText ){
			if( VBOHandlesCount + 1 > VBOHandlesSize ){
				VBOHandlesSize = (VBOHandlesCount + 1) * 2;
				VBOHandles = (VBOStructureHandle*)realloc( VBOHandles, sizeof(VBOStructureHandle) * VBOHandlesSize );
			}
			VBOStructureHandle* sh = &VBOHandles[VBOHandlesCount];
			sh->texture = lastText = s->texid;
			sh->start = count;
			if( VBOHandlesCount )
				VBOHandles[VBOHandlesCount - 1].count = count - VBOHandles[VBOHandlesCount - 1].start;
			VBOHandlesCount++;
		}*/
		if( !v ){
			first = v = new VBOStructureHandle(s->texid, s->shader, count);
		}else if( s->texid != v->texture || s->shader != v->shader ){
			v->next = new VBOStructureHandle(s->texid, s->shader, count);
			v->count = count - v->start;
			v = v->next;
		}

		vertex3farr offset = s->fixedOffset( &vpoint );
		verticles[count++].set( &offset.lb, &s->coordinates.lt, &s->clr ); // = VertexV2FT2FC4UI( offset.lb, s->coordinates.lt, &s->clr);
		verticles[count++].set( &offset.rb, &s->coordinates.rt, &s->clr ); // = VertexV2FT2FC4UI( offset.rb, s->coordinates.rt, &s->clr);
		verticles[count++].set( &offset.rt, &s->coordinates.rb, &s->clr ); // = VertexV2FT2FC4UI( offset.rt, s->coordinates.rb, &s->clr);
		verticles[count++].set( &offset.lt, &s->coordinates.lb, &s->clr ); //= VertexV2FT2FC4UI( offset.lt, s->coordinates.lb, &s->clr);
	}
	//if( VBOHandlesCount )
	//	VBOHandles[VBOHandlesCount - 1].count = count - VBOHandles[VBOHandlesCount - 1].start;
	if( v != NULL)
		v->count = count - v->start;
	(*c) = count;
	return first;
}



bool RenderManager::BuildAtlasMap( int* width, int* height )
{
	ElasticBox box = ElasticBox(minAtlasSize, maxAtlasSize);

	for( std::vector < TextureProxy* >:: iterator it = internalTextures.begin(),
			vend = internalTextures.end(); it != vend; ++it ){
		if ( ! box.InsertItem(
				&((*it)->atlasX), &((*it)->atlasY),
				(*it)->width, (*it)->height ) )
			return false;
	}

	//FIXME: class atlas
	(*width) = box.Width;
	(*height) = box.Height;

	return BuildAtlasAbsoluteMap( box.Width, box.Height );
}


bool RenderManager::BuildAtlasAbsoluteMap(float width, float height){

	float texelW = 1.0f;
	float texelH = 1.0f;

	texelW = texelW / width;
	texelH = texelH / height;

	// Build absolute map
	for( unsigned int i = 0; i < internalTextures.size(); i++ ){
		internalTextures[i]->atlas.x = static_cast<float>(internalTextures[i]->atlasX) * texelW;
		internalTextures[i]->atlas.y = static_cast<float>(internalTextures[i]->atlasY) * texelH;
		internalTextures[i]->atlas.width = static_cast<float>(internalTextures[i]->width) * texelW;
		internalTextures[i]->atlas.height = static_cast<float>(internalTextures[i]->height) * texelH;
	}

	return true;
}


bool RenderManager::BuildAtlas( GLuint* ahandle, int width, int height )
{
	return DrawToGLTexture( ahandle, width, height, &internalTextures );
}

void RenderManager::TestDrawAtlas(int x, int y, GLuint atlas)
{
	int aw, ah;
	if( !atlas ){
		atlas = atlasHandle;
		aw = atlasWidth;
		ah = atlasHeight;
	}else{
		aw = ah = maxAtlasSize;
	}
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, atlas);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0, 0.0);
		glVertex2f(x, y);
		glTexCoord2f(1, 0);
		glVertex2f(x + aw, y);
		glTexCoord2f(1, 1);
		glVertex2f(x + aw, y + ah);
		glTexCoord2f(0, 1);
		glVertex2f(x, y + ah);
	}
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

#ifdef DEBUG_DRAW_RECTANGLES
	glBegin(GL_LINE_LOOP);
	{
		glVertex2f(x, y);
		glVertex2f(x + aw, y);
		glVertex2f(x + aw, y + ah);
		glVertex2f(x, y + ah);
	}
	glEnd();
	glBegin(GL_LINES);
	for( int i=0; i< 8190; i += 86  ){
		glVertex2f(x + i, y);
		glVertex2f(x + i, y + 300);
	}
	glEnd();

#endif


}

