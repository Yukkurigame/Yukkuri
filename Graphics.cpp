#include "Graphics.h"

Graphics Graphics::graph; 

Graphics::~Graphics()
{
    LoadedImages.clear();
    Animations.clear();
}

SDL_Surface* Graphics::LoadImage(string name)
{    
    SDL_Surface* pImg = NULL;
    //Find in cache
    pImg = GetImage( name );
    if( !pImg ){
        //Not in cache. Try to load.        
        pImg = YLoader::loader.LoadImage( IMAGEPATH + name );
        if( !pImg){
            //Not loaded. Load default.
            pImg = LoadImage( "white.png" );
        }else{
            //Add in cache.
            AddImage( pImg, name);
        }
    }
    return pImg;
}

void Graphics::ApplySurface( double x, double y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect *clip = NULL )
{    
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface( source, clip, destination, &offset );
}

void Graphics::AddImage( SDL_Surface*  pImg, string name){
    LoadedImages[name] = pImg;
}

SDL_Surface* Graphics::GetImage( string name){
    map <string, SDL_Surface* >::iterator it;
    it = LoadedImages.find(name);
    if( it != LoadedImages.end() ){
        return it->second;
    }
    return NULL;
}

void Graphics::LoadAnimation( string name,  int rows, int cols, int width, int height )
{
   int border = 1;
   //Animations.erase( Animations.find(name) );
   for( int row = 0; row < rows; row++ ){
       for( int col = 0; col < cols; col++ ){
           Animations[name].push_back( GetSDLRect(  col * ( border + width ), row * ( border + height ), width, height) );
       }
   }
}

SDL_Rect Graphics::GetSDLRect( int startx, int starty, int width, int height )
{
    SDL_Rect temp_rect;
    
    temp_rect.x = startx;
    temp_rect.y = starty;
    temp_rect.w = width;
    temp_rect.h = height;
    
    return temp_rect;
}

SDL_Rect* Graphics::GetAnim( string name, int num )
{
    return &Animations[name].at(num);
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Здесь p - это адрес пиксела, который мы хотим получить */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch(bpp) {
    case 1:
        return *p;
    case 2:
        return *(Uint16 *)p;
    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
    case 4:
        return *(Uint32 *)p;
    default:
        return 0;       /* нидолжно выполниться, но позволяет избежать предупреждений */
    }
}

void Graphics::SetAlpha( SDL_Surface* sfc, int alpha){
	SDL_SetAlpha( sfc, SDL_SRCALPHA, alpha );	
} 
