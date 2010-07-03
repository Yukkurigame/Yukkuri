#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "SDL/SDL_image.h"
#include "SDL_opengl.h"
#include "GraphicsTypes.h"
#include "Font.h"
#include "Define.h"
#include "debug.h"

//TODO: add logger
#include <string.h>
#include <dirent.h>
#include <map>
using std::map;
#include <vector>
using std::vector;
#include <string>
using std::string;


class Graphics
{
	public:
		static Graphics graph;

		~Graphics();

		void openglInit();
		void openglSetup( int wwidth, int wheight );

		Texture* LoadGLTexture( string name );
		void FreeGLTexture( Texture* );

		inline void DrawGLTexture( Sprite* s ){
			DrawGLTexture( s->tex, s->vertices, s->coordinates, s->col );
		}
		void DrawGLTexture( Texture* tex, vertex3farr* vertices, coord2farr* coordinates, Color* col );

		void LoadAllTTFonts( int size );
		bool LoadTTFont( string dir, string filename, int size );
		void PrintText(string font, float size, float x, float y, const int* color, string text);

		inline Sprite* CreateGLSprite( float x, float y, float z, float width, float height ){
			return CreateGLSprite( x, y, z, 0, 0, width, height, NULL);
		}
		inline Sprite* CreateGLSprite( float x, float y, float z, float width, float height, Texture* tex ){
			return CreateGLSprite( x, y, z, 0, 0, width, height, tex);
		}
		Sprite* CreateGLSprite( float x, float y, float z, float texX, float texY,
								float width, float height, Texture* tex, short mirrored = 0, short centered =0 );

		void FreeGLSprite( Sprite* );

		coord2farr* GetCoordinates(float x1, float y1, float x2, float y2, float width, float height, short mirrored);
		vertex3farr* GetVertex(float x, float y, float z, float width, float height, short centered);

		coord2farr* GetAnimation( string , unsigned int );
		void LoadAnimation( string name, int rows, int cols, int width, int height, int texw, int texh );

		void DrawGLScene();
		void CleanGLScene();

		bool SaveScreenshot(const char *FileName);

	private:
		void AddGLTexture( Texture* , string );
		Texture* getGLTexture( string );

		SDL_Surface* LoadImage( std::string );
		SDL_Surface* OpenImage( std::string );

		font_data* GetFont( string name );
		void PrintText(const font_data &ft_font, float x, float y, float size, const int* color, const char *str);

		map < string, vector<coord2farr*> > Animations;
		map < string, Texture* > LoadedGLTextures;
		map < string, font_data* > LoadedFonts;

};


#endif // GRAPHICS_H
