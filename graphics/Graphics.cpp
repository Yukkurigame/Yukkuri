#include "Graphics.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"
#include "pngfuncs.h"
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <sys/stat.h>
#include <dirent.h>
#include "config.h"
#include "debug.h"

extern MainConfig conf;

Graphics* Graphics::graph = 0;

namespace Screenshot
{
	/*Generate screenshot name. From MPlayer */
	static int Exists( char *name )
	{
		struct stat dummy;
		if( stat( name, &dummy ) == 0 )
			return 1;
		return 0;
	}

	static void GenerateName( char* name )
	{
		int iter = 0;
		do{
			snprintf( name, 50, "screenshot%03d.png", ++iter);
		}while( Exists( name ) && iter < 1000 );
		if( Exists( name ) ){
			name[0] = '\0';
		}
	}
}

//FIXME: duplicate in Font.cpp
inline int next_p2( int a )
{
	int rval=2;
	while( rval < a ) rval <<= 1;
	return rval;
}

inline bool compareSprites( Sprite* s1, Sprite* s2 )
{
	if( s1->vertices->z == s2->vertices->z ){
		if( s1->posy == s2->posy ){
			return ( s1->posx > s2->posx );
		}
		return ( s1->posy > s2->posy );
	}
	return ( s1->vertices->z < s2->vertices->z );
}

inline bool compareRects( imageRect* s1, imageRect* s2 )
{
	if( s1->z == s2->z ){
		if( s1->y == s2->y ){
			return ( s1->x > s2->x );
		}
		return ( s1->y > s2->y );
	}
	return ( s1->z < s2->z );
}

bool Graphics::SetScreen( SDL_Surface* s )
{
	if( s == NULL )
		return false;

	screen = s;
	return true;
}

void Graphics::openglInit( )
{
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 6 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 6 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 6 );
	SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 6 );
}

void Graphics::openglSetup( int wwidth, int wheight )
{
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

}

Texture* Graphics::LoadGLTexture( string name )
{
	Texture* tex;
	Texture* cached;
	SDL_Surface* surface;


	if( name == "" )
		return NULL;

	cached = GetGLTexture( name );

	if( !cached || !cached->texture ){

		surface = LoadImage( name.c_str() );

		if( !surface ){
			debug( 3, name + " not loaded.\n" );
			delete cached;
			return NULL;
		}

		cached = CreateGlTexture( surface );

		AddGLTexture( cached, name );

		if( surface )
			SDL_FreeSurface( surface );

	}

	tex = new Texture( cached );


	return tex;
}

//FIXME: depreciated?
void Graphics::LoadAllTTFonts( int size )
{
	debug(3, "Loading fonts");
	string dirname = conf.fontsPath;
    DIR *dp;
    struct dirent *ep;
    dp = opendir (dirname.c_str());
    int success = 0;
    int files = 0;
    if( dp != NULL ){
        while ( (ep = readdir( dp ) ) != NULL) {
            string fname = string(ep->d_name);
            int d = fname.find_last_of(".");
            if(fname.substr(d + 1) == "ttf"){
            	files++;
            	fname = fname.substr(0, d);
                if ( LoadTTFont( dirname, fname, size ) )
                	success++;
            }
        }
        closedir(dp);
    }else{
    	debug(3, "\tFAIL Bad directory.");
        return;
    }
    //pdbg(3, "Done.\n");
    //FIXME: debug print
    char dbg[38];
    sprintf(dbg, "Loaded %d from %d font files.", success, files);
    debug(3, dbg);
	return;
}

bool Graphics::LoadTTFont( string dir, string name, int size )
{
	font_data* font;
	string filename;

	filename = dir + name + ".ttf";

	font = new font_data();
	//FIXME: it's cruved;
	if( font->load( filename.c_str() , size ) ){
		LoadedFonts[name][size] = font;
	}else{
		delete font;
		return false;
	}
	return true;
}

Sprite* Graphics::CreateTextSprite( string fontname, int size, float x, float y, float z, Color* color,
		string text, short cached )
{
	font_data* font = GetFont( fontname, size );
	if(!font){
		debug( 3,"Font " + fontname + " not found.\n" );
		return NULL;
	}
	return CreateTextTexture( font, x, y, z, color, text );
}

void Graphics::ChangeTextSprite( Sprite* spr, string fontname, int size, string text, short cached )
{
	float width;
	float height;
	font_data* font;
	Texture* tex;

	tex = NULL;

	font = GetFont( fontname, size );

	{ //TODO: In thread

		if( spr->tex ){
			FreeGLTexture( spr->tex );
			spr->tex = NULL;
		}

		width = height = 0;

		tex = GetTextTexture( font, text );
		if( !tex ){
			tex = new Texture();
			font->print( tex, &width, &height, text.c_str() );
		}else{
			font->size( &width, &height, text.c_str() );
		}

		spr->tex = tex;
		spr->resize( width, height );
		delete spr->coordinates;
		spr->coordinates = GetCoordinates( 0, 0, width, height, tex->w, tex->h, 0 );

		if( cached )
			SetTextTexture( tex, font, text );
	}

}

/* This function creating sprite structure with texture.
 * x, y, z - right top coordinates;
 * texX, texY - right top coordinates of texture rectangle;
 * width, height - width and height of sprite/texture rectangle;
 * tex - texture structure;
 * mirrored - rotates the image on the x axis;
 * centered - origin at the center of the object if not 0;
 * return Sprite*
 */
Sprite* Graphics::CreateGLSprite( float x, float y, float z, float texX, float texY, float width,
						float height, Texture* tex, short mirrored, short centered,  short cached )
{
	Sprite* sprite;
	coord2farr* coords;

	sprite = new Sprite();

	//get vertex
	//sprite->vertices = GetVertex( x, y, z, width, height, centered );
	sprite->vertices = GetVertex( );
	sprite->resize( width, height );
	sprite->setPosition( x, y, z );


	if(tex){ //no image - no rectangle;
		coords = GetCoordinates( texX, texY, width, height, tex->w, tex->h, mirrored);
		sprite->tex = tex;
		sprite->coordinates = coords;
		sprite->clr = &sprite->tex->clr;
	}else{
		sprite->tex = NULL;
		sprite->coordinates = NULL;
		sprite->clr = new Color( );
	}

	sprite->width = width;
	sprite->height = height;

	if(centered)
		sprite->centered = true;

	if( cached )
		GLSprites.push_back( sprite );

	return sprite;
}

void Graphics::CreateGLSpriteList( vector<Sprite* >* sprites )
{
	sort( sprites->begin(), sprites->end(), compareSprites );
	if( glIsList( MapListBase ) )
		glDeleteLists( MapListBase, 1 );
	MapListBase = glGenLists(1);
	glNewList( MapListBase, GL_COMPILE );
		//TODO: try GL_QUAD_STRIP
		for( vector<Sprite*>::iterator it = sprites->begin(), end = sprites->end(); it != end; ++it ){
			DrawGLTexture( *it );
		}
	glEndList();
}

void Graphics::CreateGLTextureAtlas( int size, imageRect rects[], int count )
{
	Uint32 rmask, gmask, bmask, amask;
	SDL_Surface* sdlAtlas;
	SDL_Surface* sdltemp;
	SDL_Rect src, dst;
	Texture* Atlas;
	string name = "Texture Atlas";
	int colcount, rowcount, texturewidth, textureheight;
	if( size <= 0 || count <= 0 )
		return;
	Atlas = NULL;
	colcount = static_cast<int>( ceil(sqrt(count)) );
	texturewidth = next_p2( colcount * size );
	colcount = texturewidth / size;
	rowcount = static_cast<int>( ceil( static_cast<float>(count) / static_cast<float>(colcount) ) );
	textureheight = next_p2( rowcount * size );
	dst.w = dst.h = src.w = src.h = size;

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000; gmask = 0x00ff0000; bmask = 0x0000ff00; amask = 0x000000ff;
	#else
	rmask = 0x000000ff; gmask = 0x0000ff00; bmask = 0x00ff0000; amask = 0xff000000;
	#endif

	sdlAtlas = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, texturewidth, textureheight, 32,
									rmask, gmask, bmask, amask);
	SDL_SetAlpha( sdlAtlas, 0, SDL_ALPHA_OPAQUE );

	for( int row = 0, cnt = 0; row < rowcount; ++row ){
		for( int col = 0; col < colcount; ++col ){
			if( cnt >= count )
				break;
			const char* imageName = rects[cnt].imageName;
			sdltemp = LoadImage( imageName );

			src.x = static_cast<Sint16>(rects[cnt].x);
			src.y = static_cast<Sint16>(rects[cnt].y);
			dst.x  = size * col;
			dst.y = size * row;

			rects[cnt].coordinates = GetCoordinates( dst.x, dst.y, size, size, texturewidth,
																	textureheight, 0);

			SDL_SetAlpha( sdltemp, 0, SDL_ALPHA_OPAQUE );
			SDL_BlitSurface( sdltemp, &src, sdlAtlas, &dst );
			SDL_FreeSurface( sdltemp );

			++cnt;
		}
	}

	Atlas = CreateGlTexture( sdlAtlas );

	SDL_FreeSurface( sdlAtlas );

	name += size + "x" + count;
	AddGLTexture( Atlas, name );

	for( int i = 0; i < count; ++i ){
		rects[i].texture = Atlas;
	}

}

//TODO: array of rects
void Graphics::AddImageRectArray( vector < imageRect* >* rects )
{
	if( rects->size() < 1 )
		return;
	sort( rects->begin(), rects->end(), compareRects );
	ImageRects = rects;
}
//TODO: everything 'll be turned into imageRects, even the sky, Allah himself.
void Graphics::RemoveImageRectArray( vector < imageRect* >* rects )
{
	if( ImageRects == rects )
		ImageRects = NULL;
}

void Graphics::FreeGLSprite( Sprite* spr )
{
	if( spr ){
		if(!spr->tex || spr->clr != &spr->tex->clr ){
			delete spr->clr;
			spr->clr = NULL;
		}
		spr->tex = NULL;
		//All textures deleted on exit;
		//if( spr->tex )
			//FreeGLTexture( spr->tex );
		if( spr->vertices ){
			delete spr->vertices;
			spr->vertices = NULL;
		}
		if( spr->coordinates ){
			FreeCoordinates( spr->coordinates );
			spr->coordinates = NULL;
		}
		//std::cout << GLSprites.size() << "! " << GLSprites.size() << " sprites! A-ha-ha-ha-ha... " << std::endl;
		for( int i = 0, end = GLSprites.size(); i < end; ++i ){
			if( GLSprites[i] == spr ){
				GLSprites.erase( GLSprites.begin() + i );
				break;
			}
		}
		delete spr;
	}
}

//FIXME: WTF?
void Graphics::FreeTextSprite( Sprite** spr )
{
	if( !spr || !(*spr) )
		return;
	for( std::map < font_data*, std::map< string, Texture* > > ::iterator it = CachedTexts.begin(),
			end = CachedTexts.end(); it != end; ++it ){
		for( std::map< string, Texture* >::iterator vit = it->second.begin(), vend = it->second.end();
				vit != vend; ++vit ){
			if( vit->second == (*spr)->tex ){
				it->second.erase(vit);
				break;
			}
		}
	}
	FreeGLTexture( (*spr)->tex );
	(*spr)->tex = NULL;
	FreeGLSprite( (*spr) );
	*spr = NULL;
}

void Graphics::SetVertex( vertex3farr* v, float x, float y, float z, float width, float height, short centered )
{
	float xp, xm, yp, ym;

	if(centered){
		float halfwidth = width/2;
		float halfheight = height/2;
		xp = x + halfwidth;
		xm = x - halfwidth;
		yp = y + halfheight;
		ym = y - halfheight;
	}else{
		xp = x + width;
		xm = x;
		yp = y + height;
		ym = y;
	}

	v->lb.x = xm;
	v->lb.y = ym;

	v->lt.x = xm;
	v->lt.y = yp;

	v->rb.x = xp;
	v->rb.y = ym;

	v->rt.x = xp;
	v->rt.y = yp;

	v->z = z;
}

coord2farr* Graphics::GetAnimation( string name, unsigned int num )
{
	//FIXME: Add check for correct index.
	if( Animations.count( name ) > 0) {
		if( num < Animations[name].size() ){
			return Animations[name].at(num);
		}else{
			return Animations[name].at(0);
		}
	}
	return NULL;
}

void Graphics::LoadAnimation( string name, int rows, int cols, int width, int height, int texw, int texh )
{
	//Animations.erase( Animations.find(name) );
	for( int row = 0; row < rows; ++row ){
		for( int col = 0; col < cols; ++col ){
			Animations[name].push_back(
					GetCoordinates( col *  width + col , row * height + row,
							width, height,
							texw, texh, 0)
				);
	   }
   }
}

void Graphics::MoveGlScene( int x, int y, int z )
{
	//FIXME: double cast
	vpoint.x = x;
	vpoint.y = y;
	vpoint.z = z;
	glTranslatef( x, y, z );
}

void Graphics::DrawGLScene()
{
	DrawImageRects( ImageRects );

	sort(GLSprites.begin(), GLSprites.end(), compareSprites);
	for( vector<Sprite*>::iterator it = GLSprites.begin(), end = GLSprites.end(); it != end; ++it ){
		if( (*it)->visible ){
			DrawGLTexture( (*it) );
		}
	}

	glLoadIdentity();
	SDL_GL_SwapBuffers();
}

void Graphics::CleanGLScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

bool Graphics::SaveScreenshot( )
{
	char Filename[52];
	int stride;
	GLubyte* pixels;
	GLubyte* swapline;
	Uint32 rmask, gmask, bmask, amask;
	SDL_Surface* output;

	Screenshot::GenerateName( Filename );
	if( Filename[0] == '\0' ){
		debug(3, "Can not get screenshot name. Too many screenshots in folder.\n");
		return false;
	}

	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000; gmask = 0x00ff0000; bmask = 0x0000ff00; amask = 0x000000ff;
	#else
	rmask = 0x000000ff; gmask = 0x0000ff00; bmask = 0x00ff0000; amask = 0xff000000;
	#endif

	stride = screen->w * 4;
	pixels = (GLubyte *) malloc( stride * screen->h );
	swapline = (GLubyte *) malloc( stride );
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, screen->w, screen->h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	// vertical flip
	for( int row = 0; row < screen->h/2; ++row ){
		memcpy( swapline, pixels + row * stride, stride );
		memcpy( pixels + row * stride, pixels + ( screen->h - row - 1 ) * stride, stride );
		memcpy( pixels + ( screen->h - row -1 ) * stride, swapline, stride );
	}
	free( swapline );

	output = SDL_CreateRGBSurfaceFrom( pixels, screen->w, screen->h, 32, screen->pitch, rmask, gmask, bmask, amask );

	if( png_save_surface(Filename, output) < 0 ){
		free( pixels );
		SDL_FreeSurface( output );
		return false;
	}
	free( pixels );
	SDL_FreeSurface( output );
	return true;
}

/* ** PRIVATE METHODS ** */

Graphics::Graphics( ){
	ImageRects = NULL;
}

Graphics::~Graphics( )
{
	for( std::map< string, vector<coord2farr*> >::iterator it = Animations.begin(), end = Animations.end();
			it != end; ++it ){
		for( vector<coord2farr*>::iterator vit = it->second.begin(), vend = it->second.end(); vit != vend; ++vit ){
			delete (*vit);
		}
		it->second.clear();
	}
	Animations.clear();
	for( std::map< string, Texture* >::iterator it = LoadedGLTextures.begin(), end = LoadedGLTextures.end();
			it != end; ++it ){
		FreeGLTexture( it->second );
		it->second = NULL;
	}
	LoadedGLTextures.clear();
	for( std::map < font_data*, std::map< string, Texture* > >::iterator it = CachedTexts.begin(),
			end = CachedTexts.end(); it != end; ++it ){
		for( std::map< string, Texture* >::iterator vit = it->second.begin(), vend = it->second.end();
				vit != vend; ++vit ){
			FreeGLTexture( vit->second );
			vit->second = NULL;
		}
		it->second.clear();
	}
	CachedTexts.clear();
	for( std::map< string, std::map< int, font_data*> >::iterator it = LoadedFonts.begin(),
			end = LoadedFonts.end(); it != end; ++it ){
		for( std::map< int, font_data*>::iterator vit = it->second.begin(), vend = it->second.end();
				vit != vend; ++vit ){
			vit->second->clean();
			delete vit->second;
		}
		it->second.clear();
	}
	LoadedFonts.clear();
}

void Graphics::DrawImageRects( vector < imageRect* >* rects )
{
	float xm, xp, ym, yp;
	coord2farr* coord;
	Texture* first;
	imageRect* temp;
	if( !rects || rects->size() < 1 || ! rects->front()->texture )
			return;
	first = rects->front()->texture;


	glBindTexture( GL_TEXTURE_2D, *first->texture );
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_QUADS);

	glColor4ub( first->clr.r, first->clr.g, first->clr.b, first->clr.a );

	for( vector< imageRect* >::iterator it = rects->begin(), end = rects->end(); it != end; ++it ){
		temp = (*it);
		if( first != temp->texture )
			continue;

		coord = temp->coordinates;

		xp = temp->x + temp->width;
		xm = temp->x;
		yp = temp->y + temp->height;
		ym = temp->y;

		if( coord ){
			//Bottom-left vertex (corner)
			glTexCoord2f( coord->lb.x, coord->lb.y );
			glVertex3f( xm, ym, temp->z );

			//Bottom-right vertex (corner)
			glTexCoord2f( coord->rb.x , coord->rb.y );
			glVertex3f( xp, ym, temp->z );

			//Top-right vertex (corner)
			glTexCoord2f( coord->rt.x , coord->rt.y );
			glVertex3f( xp, yp, temp->z );

			//Top-left vertex (corner)
			glTexCoord2f( coord->lt.x , coord->lt.y );
			glVertex3f( xm, yp, temp->z );
		}else{
			glVertex3f( xm, ym, temp->z ); //Bottom-left
			glVertex3f( xp, ym, temp->z ); //Bottom-right
			glVertex3f( xp, yp, temp->z ); //Top-right
			glVertex3f( xm, yp, temp->z ); //Top-left
		}
	}

	glEnd();
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void Graphics::AddGLTexture( Texture* texture, string name)
{
	Texture* cached;
	if( !texture )
		return;
	cached = GetGLTexture( name );
	if( cached == texture )
		return;
	FreeGLTexture( cached );
	LoadedGLTextures[name] = texture;
}

Texture* Graphics::GetGLTexture( string name )
{
	std::map <string, Texture* >::iterator it;
	it = LoadedGLTextures.find(name);
	if( it != LoadedGLTextures.end() ){
		return it->second;
	}
	return NULL;
}

void Graphics::DrawGLTexture( Texture* tex, vertex3farr* vertices, coord2farr* coordinates, Color* col, bool fixed)
{
	float offsetx, offsety;
	offsetx = offsety = 0;
	if( fixed ){
		offsetx = vpoint.x;
		offsety = vpoint.y;
	}

//Draw border around texture
#ifdef DEBUG_DRAW_RECTANGLES
	glDisable(GL_TEXTURE_2D);

	glColor3ub( 255, 255, 0);

	glBegin(GL_LINE_LOOP);
		glVertex3i( vertices->lb.x - offsetx - 1, vertices->lb.y - offsety - 1, vertices->z ); //Bottom-left
		glVertex3i( vertices->rb.x - offsetx + 1, vertices->rb.y - offsety - 1, vertices->z ); //Bottom-right
		glVertex3i( vertices->rt.x - offsetx + 1, vertices->rt.y - offsety + 1, vertices->z ); //Top-right
		glVertex3i( vertices->lt.x - offsetx - 1, vertices->lt.y - offsety + 1, vertices->z ); //Top-left
	glEnd();

	glEnable(GL_TEXTURE_2D);
#endif

	if(tex && tex->texture){
		glBindTexture( GL_TEXTURE_2D, *tex->texture );
	}else{
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	//Sprite color
	glColor4ub( col->r, col->g, col->b, col->a );
	if(coordinates){
		glBegin( GL_QUADS );
			//Bottom-left vertex (corner)
			glTexCoord2f( coordinates->lb.x, coordinates->lb.y );
			glVertex3f( vertices->lb.x - offsetx, vertices->lb.y - offsety, vertices->z );

			//Bottom-right vertex (corner)
			glTexCoord2f( coordinates->rb.x , coordinates->rb.y );
			glVertex3f( vertices->rb.x - offsetx, vertices->rb.y - offsety, vertices->z );

			//Top-right vertex (corner)
			glTexCoord2f( coordinates->rt.x , coordinates->rt.y );
			glVertex3f( vertices->rt.x - offsetx, vertices->rt.y - offsety, vertices->z );

			//Top-left vertex (corner)
			glTexCoord2f( coordinates->lt.x , coordinates->lt.y );
			glVertex3f( vertices->lt.x - offsetx, vertices->lt.y - offsety, vertices->z );
		glEnd();
	}else{
		glBegin( GL_QUADS ); //draw rectangle;
			glVertex3f( vertices->lb.x - offsetx, vertices->lb.y - offsety, vertices->z ); //Bottom-left
			glVertex3f( vertices->rb.x - offsetx, vertices->rb.y - offsety, vertices->z ); //Bottom-right
			glVertex3f( vertices->rt.x - offsetx, vertices->rt.y - offsety, vertices->z ); //Top-right
			glVertex3f( vertices->lt.x - offsetx, vertices->lt.y - offsety, vertices->z ); //Top-left
		glEnd();
	}
}

void Graphics::FreeGLTexture( Texture* tex )
{
	if( tex ){
		if( tex->texture ){
			glDeleteTextures( 1, tex->texture );
			tex->texture = NULL;
		}
		delete tex;
	}
}

coord2farr* Graphics::GetCoordinates(float x1, float y1, float x2, float y2,
										float width, float height, short mirrored)
{
	coord2farr* c;

	if( x1 == 0 && x2 == 0 && y1 == 0 && y2 == 0 )
		return NULL;

	c = new coord2farr();

	float cx1 = x1 / width;
	float cx2 = ( x1 + x2 ) / width;
	float cy2 = y1 / height;
	float cy1 = ( y1 + y2 ) / height;

	if(mirrored){
		c->lt.x = cx2;
		c->lt.y = cy2;

		c->lb.x = cx2;
		c->lb.y = cy1;

		c->rt.x = cx1;
		c->rt.y = cy2;

		c->rb.x = cx1;
		c->rb.y = cy1;
	}else{
		c->lt.x = cx1;
		c->lt.y = cy2;

		c->lb.x = cx1;
		c->lb.y = cy1;

		c->rt.x = cx2;
		c->rt.y = cy2;

		c->rb.x = cx2;
		c->rb.y = cy1;
	}

	return c;
}

void Graphics::FreeCoordinates( coord2farr* coord )
{
	for( std::map< string, vector<coord2farr*> >::iterator it = Animations.begin(), end = Animations.end();
			it != end; ++it ){
		for( vector<coord2farr*>::iterator vit = it->second.begin(), vend = it->second.end(); vit != vend; ++vit ){
			if( coord == (*vit) )
				return; //Chached coordinates deleted on exit;
		}
	}
	delete coord;
}

inline vertex3farr* Graphics::GetVertex(  )
{
	vertex3farr* v;
	v = new vertex3farr();

	return v;
}

vertex3farr* Graphics::GetVertex( float x, float y, float z, float width, float height, short centered )
{
	vertex3farr* v;
	v = GetVertex();

	SetVertex( v, x, y, z, width, height, centered );

	return v;
}

Texture* Graphics::GetTextTexture( font_data* font, string text )
{
	if( CachedTexts.count(font) > 0 ){
		if( CachedTexts[font].count(text) > 0 )
			return CachedTexts[font][text];
	}
	return NULL;
}

void Graphics::SetTextTexture( Texture* tex, font_data* font, string text )
{
	if( CachedTexts.count(font) > 0  &&  CachedTexts[font].count(text) > 0 ){
		FreeGLTexture( CachedTexts[font][text] );
		CachedTexts[font][text] = NULL;
	}
	CachedTexts[font][text] = tex;
}

Texture* Graphics::CreateGlTexture( SDL_Surface* surface )
{
	Texture* tex;
	GLuint* image;
	GLint  nOfColors;
	GLenum texture_format;

	tex = new Texture();

	/*if( surface->format->colorkey != 0 ){
		//FIXME: Indexed images support.
		debug( 1, "BIDA while loading " + name + "! Indexed images not supported yet!\n" );
		texture_format = GL_COLOR_INDEX;
	}else{*/
		nOfColors = surface->format->BytesPerPixel;

		if( nOfColors == 4 ){     // contains an alpha channel
			if( surface->format->Rmask == 0x000000ff )
				texture_format = GL_RGBA;
			else
				texture_format = GL_BGRA;
		}else if( nOfColors == 3 ){     // no alpha channel
			if (surface->format->Rmask == 0x000000ff)
				texture_format = GL_RGB;
			else
				texture_format = GL_BGR;
		}else{
			debug(3, "Image is not truecolor..  this will probably break.\n");
			// this error should not go unhandled
		}
	//}

	tex->w = surface->w;
	tex->h = surface->h;

	// Have OpenGL generate a texture object handle for us
	image = new GLuint();
	glGenTextures( 1, image );

	// Bind the texture object
	glBindTexture( GL_TEXTURE_2D, *image );

	// Set the texture's stretching properties
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// Edit the texture object's image data using the information SDL_Surface gives us
	glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0,
					texture_format, GL_UNSIGNED_BYTE, surface->pixels );

	tex->texture = image;

	return tex;

}

//TODO: one more cache?
SDL_Surface* Graphics::LoadImage( const char* name )
{
	SDL_Surface* pImg = NULL;
	char path[ conf.imagePath.size( ) + strlen( name ) ];
	strcpy( path, conf.imagePath.c_str() );
	strcat( path, name );
	pImg = OpenImage( path );
	if( !pImg ){
		//Not loaded.
		if( name != conf.defaultImage.c_str() ){
			strcpy( path, conf.defaultImage.c_str() );
			pImg = LoadImage( path );
		}else{
			return NULL; // Default already tried. Break.
		}
	}
	return pImg;
}

/** Load image file
    @return A pointer to the SDL_Surface surface.
**/
SDL_Surface* Graphics::OpenImage( const char* filename )
{
	SDL_Surface* loadedImage = NULL;
	SDL_Surface* optimizedImage = NULL;
	char dbg[ strlen(filename) + 30 ];
	loadedImage = IMG_Load( filename );

	if( loadedImage != NULL ){
		if( ( loadedImage->w & (loadedImage->w - 1) ) != 0 ){
			strcpy( dbg, filename );
			strcat( dbg, " width is not a power of 2!\n" );
        	debug( 3, dbg );
		}else if( (loadedImage->h & (loadedImage->h - 1) ) != 0 ){
			strcpy( dbg, filename );
			strcat( dbg, " height is not a power of 2!\n" );
			debug( 3, dbg );
		}else{
			//FIXME: no-alpha images
			if( loadedImage->format->Amask )
				optimizedImage = SDL_DisplayFormatAlpha( loadedImage );
			else
				optimizedImage = SDL_DisplayFormatAlpha( loadedImage );
		}
		SDL_FreeSurface( loadedImage );
	}else{
		strcpy( dbg, "Could not load " );
		strcat( dbg, filename );
		strcat( dbg, "\n" );
		debug(3, dbg );
	}
	return optimizedImage;
}

font_data* Graphics::GetFont( string name, int size  )
{
	if( !LoadedFonts.count(name) || !LoadedFonts[name].count(size) ){
		if( !LoadTTFont( conf.fontsPath, name, size ) )
			return NULL;
	}
	return LoadedFonts[name][size];
}

Sprite* Graphics::CreateTextTexture( font_data* ftfont, float x, float y, float z, Color* color, string str, short cached )
{

	Sprite* ret;
	Texture* tex;
	float width;
	float height;

	if( str == "" )
		return NULL;

	ret = NULL;

	{ //TODO: In thread
		width = height = 0;

		tex = GetTextTexture( ftfont, str );

		if( !tex ){
			tex = new Texture();
			ftfont->print( tex, &width, &height, str.c_str() );
			tex->clr.set( color );
		}else{
			ftfont->size( &width, &height, str.c_str() );
		}

		ret = CreateGLSprite( x, y, z, 0, 0, width, height, tex );

		if( cached )
			SetTextTexture( tex, ftfont, str );
	}

	return ret;
}

// This function for debug only yet.
void Graphics::DrawSDLSurface( SDL_Surface* surface )
{
	int bytes;

	if( !surface )
		return;

	bytes = surface->pitch / surface->w;

	glRasterPos3i( 0, 0, 1 );

	glDisable( GL_TEXTURE_2D );

	switch( bytes ){
		case 4:
		#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			glDrawPixels( surface->w, surface->h, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, surface->pixels );
		#else
			glDrawPixels( surface->w, surface->h, GL_BGRA, GL_UNSIGNED_BYTE, surface->pixels );
		#endif
			break;
		case 3:
			glDrawPixels( surface->w, surface->h, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels );
			break;
		case 2:
			glDrawPixels( surface->w, surface->h, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, surface->pixels );
			break;
	}

	glEnable( GL_TEXTURE_2D );

}
