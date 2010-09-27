#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"
#include "GraphicsTypes.h"
#include "Font.h"
#include "Camera.h"
#include "Define.h"
#include "debug.h"

//TODO: add logger
#include <map>
#include <vector>
using std::vector;
#include <string>
using std::string;


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
	inline Sprite* CreateGLSprite( float x, float y, float z, float width, float height ){
		return CreateGLSprite( x, y, z, 0, 0, width, height, NULL );
	}
	inline Sprite* CreateGLSprite(	float x, float y, float z, float width, float height, Texture* tex ){
		return CreateGLSprite( x, y, z, 0, 0, width, height, tex );
	}
	Sprite* CreateGLSprite( float x, float y, float z, float texX, float texY, float width, float height,
							Texture* tex, short mirrored = 0, short centered = 0, short cached = 1 );

	void CreateGLSpriteList( vector<Sprite* >* sprites );

	void FreeGLSprite( Sprite* );
	void FreeTextSprite( Sprite** );

	void SetVertex( vertex3farr* v, float x, float y, float z, float width, float height, short centered );

	coord2farr* GetAnimation( string , unsigned int );
	void LoadAnimation( string name, int rows, int cols, int width, int height, int texw, int texh );

	void MoveGlScene( float x, float y, float z );
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
	void AddGLTexture( Texture* , string );
	Texture* GetGLTexture( string );
	inline void DrawGLTexture( Sprite* s ){
		if( !s ) return;
		DrawGLTexture( s->tex, s->vertices, s->coordinates, s->clr, s->fixed );
	}
	void DrawGLTexture( Texture* tex, vertex3farr* vertices, coord2farr* coordinates, Color* col, bool fixed = true );
	void FreeGLTexture( Texture* );

	coord2farr* GetCoordinates( float x1, float y1, float x2, float y2, float width, float height, short mirrored );
	void FreeCoordinates( coord2farr* );

	vertex3farr* GetVertex( float x, float y, float z, float width, float height, short centered );

	Texture* GetTextTexture( font_data* font, string text );
	void SetTextTexture( Texture* tex, font_data* font, string text );

	SDL_Surface* LoadImage( std::string );
	SDL_Surface* OpenImage( std::string );

	font_data* GetFont( string name, int size);
	Sprite* CreateTextTexture( font_data* ftfont, float x, float y, float z, Color* color, string str, short cached=0 );


	vector < Sprite* > GLSprites;
	std::map < string, vector< coord2farr* > > Animations;
	std::map < string, Texture* > LoadedGLTextures;
	std::map < string, std::map< int, font_data* > > LoadedFonts;
	std::map < font_data*, std::map< string, Texture* > > CachedTexts;

	GLuint MapListBase;
};


#endif // GRAPHICS_H
