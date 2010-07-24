#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"
#include "GraphicsTypes.h"
#include "Font.h"
#include "Define.h"
#include "debug.h"

//TODO: add logger
#include <map>
using std::map;
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

	bool SetScreen( SDL_Surface* s );

	void openglInit();
	void openglSetup( int wwidth, int wheight );

	Texture* LoadGLTexture( string name );
	void FreeGLTexture( Texture* );

	inline void DrawGLTexture( Sprite* s ){
		DrawGLTexture( s->tex, s->vertices, s->coordinates, s->clr );
	}
	void DrawGLTexture( Texture* tex, vertex3farr* vertices, coord2farr* coordinates, Color* col );

	void LoadAllTTFonts( int size );
	bool LoadTTFont( string dir, string filename, int size );
	void PrintText(string font, int size, float x, float y, float z, const int* color, string text);

	inline Sprite* CreateGLSprite( string name ){
		return CreateGLSprite( LoadGLTexture( name ) );
	}
	inline Sprite* CreateGLSprite( Texture* tex ){
		return CreateGLSprite( 0, 0, 0, 0, 0, 0, 0, tex );
	}
	inline Sprite* CreateGLSprite( float x, float y, float z, float width, float height ){
		return CreateGLSprite( x, y, z, 0, 0, width, height, NULL);
	}
	inline Sprite* CreateGLSprite( float x, float y, float z, float width, float height, Texture* tex ){
		return CreateGLSprite( x, y, z, 0, 0, width, height, tex);
	}
	Sprite* CreateGLSprite( float x, float y, float z, float texX, float texY,
							float width, float height, Texture* tex, short mirrored = 0, short centered =0 );

	void FreeGLSprite( Sprite* );

	void SetVertex( vertex3farr* v, float x, float y, float z, float width, float height, short centered );

	coord2farr* GetAnimation( string , unsigned int );
	void LoadAnimation( string name, int rows, int cols, int width, int height, int texw, int texh );

	void DrawGLScene();
	void CleanGLScene();

	bool SaveScreenshot( );

private:
	Graphics() {};
	~Graphics();
	Graphics( const Graphics& );
	Graphics& operator=(const Graphics&);
	static Graphics* graph;

	SDL_Surface* screen;
	void AddGLTexture( Texture* , string );
	Texture* getGLTexture( string );

	coord2farr* GetCoordinates( float x1, float y1, float x2, float y2, float width, float height, short mirrored );
	vertex3farr* GetVertex( float x, float y, float z, float width, float height, short centered );

	SDL_Surface* LoadImage( std::string );
	SDL_Surface* OpenImage( std::string );

	font_data* GetFont( string name, int size);
	void PrintText( const font_data &ft_font, float x, float y, float z, int size, const int* color, const char *str );

	vector < Sprite* > GLSprites;
	map < string, vector<coord2farr*> > Animations;
	map < string, Texture* > LoadedGLTextures;
	map < string, map< int, font_data*> > LoadedFonts;

};


#endif // GRAPHICS_H
