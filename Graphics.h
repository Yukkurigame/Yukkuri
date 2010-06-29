#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "SDL/SDL_image.h"
#include "SDL_opengl.h"
#include "GraphicsTypes.h"
#include "Font.h"
#include "Define.h"

//TODO: add logger
#include <iostream>
#include <string.h>
using std::cout;
using std::endl;
#include <map>
using std::map;
#include <vector>
using std::vector;
#include <string>
using std::string;


class Graphics
{
	public:
		~Graphics();
		static Graphics graph;

		void openglInit();
		void openglSetup( int wwidth, int wheight );

		Texture* LoadGLTexture( string name );
		void DrawGLTexture( Sprite* s );
		void DrawGLTexture( Texture* tex, vertex3farr* vertices, coord2farr* coordinates );

		bool LoadTTFont( string filename, int size );
		font_data* GetFont( string name ); //TODO: private?
		void PrintText(string font, float x, float y, string text);
		void PrintText(const font_data &ft_font, float x, float y, const char *str); //TODO: private?

		Sprite* CreateGLSprite( float x, float y, float z, float texX, float texY,
								float width, float height, Texture* tex, short mirrored);

		coord2farr* GetAnimation( string , unsigned int );
		void LoadAnimation( string name, int rows, int cols, int width, int height, int texw, int texh );


		SDL_Surface* LoadImage( std::string );
		SDL_Surface* OpenImage( std::string );
		//void ApplySurface( double, double, SDL_Surface*, SDL_Surface* , SDL_Rect* );

		//SDL_Rect* GetAnim( string name,  int num );
		//SDL_Rect GetSDLRect( int startx, int starty, int width, int height );
		//void SetAlpha( SDL_Surface* , int );

		void CleanGLScene();
		void DrawGLScene();

		vertex3farr* GetVertex(float x, float y, float z, float width, float height);
		coord2farr* GetCoordinates(float x1, float y1, float x2, float y2, float width, float height, short mirrored);

	private:
		map < string, vector<coord2farr*> > Animations;

		void AddGLTexture( Texture* , string );
		Texture* getGLTexture( string );

		map < string, Texture* > LoadedGLTextures;

		map < string, font_data* > LoadedFonts;

};


#endif // GRAPHICS_H
