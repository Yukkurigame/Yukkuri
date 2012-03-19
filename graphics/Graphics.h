#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "SDL/SDL.h"
#include "GraphicsTypes.h"
#include "Font.h"
#include "Camera.h"
#include "Define.h"
#include "debug.h"
#include "safestring.h"

//TODO: add logger
#include <map>
#include <vector>
using std::vector;

using std::string;

struct ViewPoint {
	int x;
	int y;
	int z;
	ViewPoint(): x(), y(), z() {};
};

class Graphics
{
public:
	static Graphics* Instance( ){
		if( !graph )
			graph = new Graphics();
		return graph;
	}

	static void Destroy( ){
		delete graph;
		graph = 0;
	}

	void test( );

	bool SetScreen( SDL_Surface* s );

	void openglInit();
	void openglSetup( int wwidth, int wheight );

	Texture* LoadGLTexture( string name );

	GLuint* LoadGLTexture( string path, int width, int height, int offsetx, int offsety );

	void LoadAllTTFonts( int size );
	bool LoadTTFont( string dir, string filename, int size );
	Sprite* CreateTextSprite( string font, int size, float x, float y, float z, Color* color, string text, short cached=0 );
	void ChangeTextSprite( Sprite* spr, string fontname, int size, string text, short cached=0 );

	inline Sprite* CreateGLSprite( string name ){
		return CreateGLSprite( LoadGLTexture( name ) );
	}
	inline Sprite* CreateGLSprite( Texture* tex ){
		return CreateGLSprite( 20, 20, 0, 0, 0, 0, 0, tex );
	}
	inline Sprite* CreateGLSprite( float x, float y, float z, int width, int height ){
		return CreateGLSprite( x, y, z, 0, 0, width, height, NULL );
	}
	inline Sprite* CreateGLSprite(	float x, float y, float z, int width, int height, Texture* tex ){
		return CreateGLSprite( x, y, z, 0, 0, width, height, tex );
	}
	Sprite* CreateGLSprite( float x, float y, float z, float texX, float texY, int width, int height,
							Texture* tex, short mirrored = 0, short centered = 0, short cached = 1 );

	void CreateGLSpriteList( vector<Sprite* >* sprites );
	void CreateGLTextureAtlas( int size, imageRect rects[], int count );

	bool LoadTextures( );

	void AddTexture( string name );
	void AddTexture( string id, string name, string path, int w, int h, int ox, int oy, int rows, int cols);

	bool CreateAtlas( );
	// To private funcs
	bool BuildAtlasMap();
	bool BuildAtlas();

	void AddImageRectArray( vector < imageRect* >* rects );
	void RemoveImageRectArray( vector < imageRect* >* rects );

	void FreeGLSprite( Sprite* );
	void FreeTextSprite( Sprite** );

	void SetVertex( vertex3farr* v, float x, float y, float z, float width, float height, short centered );

	coord2farr* GetAnimation( string , unsigned int );

	inline void LoadAnimation( string name, int rows, int cols, int width, int height, Sprite* s ){
		if( s->tex )
			return LoadAnimation( name, rows, cols, width, height, s->tex->w, s->tex->h );
		debug( 3, "No animation for " + name + "\n" );
		return LoadAnimation( name, rows, cols, width, height, 0, 0 );
	}
	void LoadAnimation( string name, int rows, int cols, int width, int height, int texw, int texh );

	void MoveGlScene( int x, int y, int z );
	void DrawGLScene();
	void CleanGLScene();

	bool SaveScreenshot( );

private:
	Graphics();
	~Graphics();
	Graphics( const Graphics& );
	Graphics& operator=(const Graphics&);
	static Graphics* graph;

	SDL_Surface* screen;

	bool LoadExtensions( );

	void DrawImageRects( vector < imageRect* >* rects );

	void AddGLTexture( Texture* , string );
	Texture* GetGLTexture( string );
	inline void DrawGLTexture( Sprite* s ){
		if( !s ) return;
		DrawGLTexture( s->tex, s->vertices, s->coordinates, s->clr, s->fixed );
	}
	void DrawGLTexture( Texture* tex, vertex3farr* vertices, coord2farr* coordinates, Color* col, bool fixed = true );
	void FreeGLTexture( Texture* );

	coord2farr* GetCoordinates( float x1, float y1, float x2, float y2, int width, int height, short mirrored );
	void FreeCoordinates( coord2farr* );

	inline vertex3farr* GetVertex(  );
	vertex3farr* GetVertex( float x, float y, float z, int width, int height, short centered );

	Texture* GetTextTexture( font_data* font, string text );
	void SetTextTexture( Texture* tex, font_data* font, string text );

	Texture* CreateGlTexture( SDL_Surface* );

	SDL_Surface* GetSDLImage( string name );
	SDL_Surface* LoadImage( const char* );
	SDL_Surface* OpenImage( const char* );

	font_data* GetFont( string name, int size);
	Sprite* CreateTextTexture( font_data* ftfont, float x, float y, float z, Color* color, string str, short cached=0 );

	void DrawSDLSurface( SDL_Surface* surface );


	int minAtlasSize;
	int maxAtlasSize;
	int atlasWidth, atlasHeight;

	GLuint* atlasHandle;

	TextureInfo textures[];
	vector < TextureS* > internalTextures;
	std::map < string, SDL_Surface* > imagesCache;


	vector < Sprite* > GLSprites;

	std::map < string, vector< coord2farr* > > Animations;
	std::map < string, Texture* > LoadedGLTextures;
	std::map < string, std::map< int, font_data* > > LoadedFonts;
	std::map < font_data*, std::map< string, Texture* > > CachedTexts;

	vector < imageRect* >* ImageRects;

	GLuint MapListBase;

	ViewPoint vpoint;

};


#endif // GRAPHICS_H
