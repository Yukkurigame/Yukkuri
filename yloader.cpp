#include "yloader.h"

int YLoader::Init( int num)
{
    return num;
}

/** Load image file
    @return A pointer to the SDL_Surface surface.
**/
SDL_Surface* YLoader::LoadImage( std::string filename )
{
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;
    loadedImage = IMG_Load( filename.c_str() );
    
    if( loadedImage != NULL )
    {
        if( loadedImage->format->Amask ) {
            optimizedImage = SDL_DisplayFormatAlpha( loadedImage );
        } else {
            optimizedImage = SDL_DisplayFormat( loadedImage );
        }
        SDL_FreeSurface( loadedImage );
    }

    return optimizedImage;
}

/*
int YLoader::LoadImage( std::string filename )
{
SDL_Surface *surface; // this surface will tell us the details of the image
 
    if ( (surface = IMG_Load(“flower.png”)) ) {
        // Check that the image’s width is a power of 2
        if ( (surface->w & (surface->w – 1)) != 0 ) {
            printf(“warning: image.bmp’s width is not a power of 2\n“);
        }
        // Also check if the height is a power of 2
        if ( (surface->h & (surface->h – 1)) != 0 ) {
            printf(“warning: image.bmp’s height is not a power of 2\n“);
        }
        //get number of channels in the SDL surface
        nofcolors=surface->format->BytesPerPixel;
        //contains an alpha channel
        if(nofcolors==4)
        {
            if(surface->format->Rmask==0×000000ff)
                texture_format=GL_RGBA;
            else
                texture_format=GL_BGRA;
            }
        else if(nofcolors==3) //no alpha channel
        {
            if(surface->format->Rmask==0×000000ff)
                texture_format=GL_RGB;
            else
                texture_format=GL_BGR;
            }
        else
        {
            printf(“warning: the image is not truecolor…this will break “);
        }
        // Have OpenGL generate a texture object handle for us
        glGenTextures( 1, &texture );
        // Bind the texture object
        glBindTexture( GL_TEXTURE_2D, texture );
        // Set the texture’s stretching properties
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexImage2D( GL_TEXTURE_2D, 0, nofcolors, surface->w, surface->h, 0,
            texture_format, GL_UNSIGNED_BYTE, surface->pixels );
    }
    else {
        printf(“SDL could not load image.bmp: %s\n“, SDL_GetError());
        SDL_Quit();
        return 1;
    }
    // Free the SDL_Surface only if it was successfully created
    if ( surface ) {
        SDL_FreeSurface( surface );
    }
}
*/


void YLoader::LoadAnimation(int rows, int cols, int width, int height)
{
   int border = 1;   
   for( int row = 0; row < rows; row++ ){
       for( int col = 0; col < cols; col++ ){           
           SDL_Rect temp;
           temp = GetSDLRect(  col * ( border + width ), row * ( border + height ), width, height);
           animation.push_back(temp);
       }
   }
}

/** 
**/
SDL_Rect YLoader::GetSDLRect( int startx, int starty, int width, int height )
{
    SDL_Rect temp_rect;
    
    
    temp_rect.x = startx;
    temp_rect.y = starty;
    temp_rect.w = width;
    temp_rect.h = height;
    
    return temp_rect;
}

SDL_Rect* YLoader::GetAnim( int num )
{
    return &animation.at(num);
}