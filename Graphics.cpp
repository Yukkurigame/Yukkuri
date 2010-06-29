#include "Graphics.h"

Graphics Graphics::graph;

Graphics::~Graphics()
{
	//FIXME: плохо так. Надо glDeleteTextures( 1, &texture );
	//LoadedImages.clear();
	//Animations.clear();
}

void Graphics::openglInit( )
{
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
}

void Graphics::openglSetup( int wwidth, int wheight )
{
	glEnable( GL_TEXTURE_2D );

	glViewport( 0, 0, wwidth, wheight );

	glClear( GL_COLOR_BUFFER_BIT );
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

	glClearColor( 100.0, 200.0, 0.0, 0.0f );

	//glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	//glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glOrtho(0.0f, wwidth, wheight, 0.0f, -1.0f, 1.0f);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

Texture* Graphics::LoadGLTexture( string name )
{
	Texture* tex;
	SDL_Surface* surface;
	GLuint* image;
	GLint  nOfColors;
	GLenum texture_format;

	tex = getGLTexture(name);

	if( !tex || !tex->texture ){

		tex = new Texture();

		surface = LoadImage(name);

		if( !surface ){
			cout << name << " not loaded" << endl;
			return NULL;
		}

		nOfColors = surface->format->BytesPerPixel;

		if(nOfColors == 4){     // contains an alpha channel
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
			cout << name << " is not truecolor..  this will probably break" << endl;
			// this error should not go unhandled
		}

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

		if( surface )
			SDL_FreeSurface( surface );

		AddGLTexture(tex, name);

	}

	return tex;
}

SDL_Surface* Graphics::LoadImage( string name )
{
	SDL_Surface* pImg = NULL;
	pImg = OpenImage( IMAGEPATH + name );
	if( !pImg ){
		//Not loaded.
		if( name != DEFAULT_IMAGE )
			pImg = LoadImage( DEFAULT_IMAGE ); // Load default.
		else
			return NULL; // Default already tried. Break.
	}
	return pImg;
}


/** Load image file
    @return A pointer to the SDL_Surface surface.
**/
SDL_Surface* Graphics::OpenImage( string filename )
{
    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;
    loadedImage = IMG_Load( filename.c_str() );

    if( loadedImage != NULL ){
        if( loadedImage->format->Amask ) {
            optimizedImage = SDL_DisplayFormatAlpha( loadedImage );
        } else {
            optimizedImage = SDL_DisplayFormat( loadedImage );
        }
        SDL_FreeSurface( loadedImage );
    }else{
        cout << "Could not load " << filename << endl;
        return NULL;
    }

    if( ( optimizedImage->w & (optimizedImage->w - 1) ) != 0 ){
    	cout << filename << " width is not a power of 2" << endl;
    	return NULL;
    }
    if( (optimizedImage->h & (optimizedImage->h - 1) ) != 0 ){
    	cout << filename << " height is not a power of 2" << endl;
    	return NULL;
    }

    return optimizedImage;
}

/*
void Graphics::ApplySurface( double x, double y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect *clip = NULL )
{
	SDL_Rect offset;
	offset.x = x;
	offset.y = y;
	SDL_BlitSurface( source, clip, destination, &offset );
}
*/


void Graphics::DrawGLTexture( Sprite* s )
{
	DrawGLTexture( s->tex, s->vertices, s->coordinates );
}


void Graphics::DrawGLTexture( Texture* tex, vertex3farr* vertices, coord2farr* coordinates)
{
	glBindTexture( GL_TEXTURE_2D, *tex->texture );
	glBegin( GL_QUADS );
		//Bottom-left vertex (corner)
		glTexCoord2f( coordinates->lb.x, coordinates->lb.y );
		glVertex3f( vertices->lb.x, vertices->lb.y, vertices->z );

		//Bottom-right vertex (corner)
		glTexCoord2f( coordinates->rb.x , coordinates->rb.y );
		glVertex3f( vertices->rb.x, vertices->rb.y, vertices->z );

		//Top-right vertex (corner)
		glTexCoord2f( coordinates->rt.x , coordinates->rt.y );
		glVertex3f( vertices->rt.x, vertices->rt.y, vertices->z );

		//Top-left vertex (corner)
		glTexCoord2f( coordinates->lt.x , coordinates->lt.y );
		glVertex3f( vertices->lt.x, vertices->lt.y, vertices->z );
	glEnd();
}

bool Graphics::LoadTTFont( string name, int size )
{
	font_data* font;
	string filename;

	filename = "data/shared/" + name + ".ttf"; //TODO: add FONTPATH

	font = new font_data();
	//FIXME: it's cruved;
	if( font->load( filename.c_str() , size ) ){
		LoadedFonts[name] = font;
	}else{
		delete font;
		return false;
	}
	return true;
}

font_data* Graphics::GetFont( string name )
{
	map <string, font_data* >::iterator it;
	it = LoadedFonts.find(name);
	if( it != LoadedFonts.end() ){
		return it->second;
	}
	return NULL;
}

void Graphics::PrintText( string fontname, float x, float y, string text)
{
	font_data* font = GetFont(fontname);
	if(!font)
		return;
	PrintText( *font, x, y, text.c_str() );
}

void Graphics::PrintText(const font_data &ft_font, float x, float y, const char *str)
{

	if( str == NULL )
		return;

	// We want a coordinate system where things coresponding to window pixels.
	//pushScreenCoordinateMatrix();

	GLuint font=ft_font.list_base;

	float h=ft_font.h/.63f;	//We make the height about 1.5* that of

	char text[256];	// Holds string

	strcpy(text, str);

	vector<string> lines;

	//Split lines by /n
	char * token;
	token = strtok(text, "\n"); //По моему так проще, чем предлагают.
	while( token != 0 ){
		string line = token;
		lines.push_back(line);
		token = strtok(NULL, "\n");
	}

	//Here is some code to split the text that we have been
	//given into a set of lines.
	//const char *start_line=text;

	//const char* c = text;

	/*
	for(;*c;++c) {
		if(*c=='\n') {
			string line;
			for(const char *n=start_line;n<c;n++) line.append(1,*n);
			lines.push_back(line);
			start_line=c+1;
		}
	}
	if(start_line) {
		string line;
		for(const char *n=start_line;n<c;n++) line.append(1,*n);
		lines.push_back(line);
	}*/

	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
	glColor3ub(0xff,0,0); //TODO: colored fonts
	glScalef(1,-1,1); //Reversed text. wtf?
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glListBase(font);

	float modelview_matrix[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	//This is where the text display actually happens.
	//For each line of text we reset the modelview matrix
	//so that the line's text will start in the correct position.
	//Notice that we need to reset the matrix, rather than just translating
	//down by h. This is because when each character is
	//draw it modifies the current matrix so that the next character
	//will be drawn immediatly after it.
	for( unsigned int i=0; i<lines.size(); ++i) {
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(x,y-h*i,0);
		glMultMatrixf(modelview_matrix);
	//  The commented out raster position stuff can be useful if you need to
	//  know the length of the text that you are creating.
	//  If you decide to use it make sure to also uncomment the glBitmap command
	//  in make_dlist().
	//	glRasterPos2f(0,0);
		glCallLists(lines[i].length(), GL_UNSIGNED_BYTE, lines[i].c_str());
	//	float rpos[4];
	//	glGetFloatv(GL_CURRENT_RASTER_POSITION ,rpos);
	//	float len=x-rpos[0];
		glPopMatrix();
	}

	glPopAttrib();

	//pop_projection_matrix();

}


/// A fairly straight forward function that pushes
/// a projection matrix that will make object world
/// coordinates identical to window coordinates.
inline void pushScreenCoordinateMatrix() {
	glPushAttrib(GL_TRANSFORM_BIT);
	GLint	viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(viewport[0],viewport[2],viewport[1],viewport[3]);
	glPopAttrib();
}

/// Pops the projection matrix without changing the current
/// MatrixMode.
inline void pop_projection_matrix() {
	glPushAttrib(GL_TRANSFORM_BIT);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glPopAttrib();
}



Sprite* Graphics::CreateGLSprite( float x, float y, float z, float texX, float texY,
										float width, float height, Texture* tex, short mirrored)
{
	Sprite* sprite;
	vertex3farr* vertex;
	coord2farr* coords;

	sprite = new Sprite();

	//get vertex
	vertex = GetVertex( x, y, z, width, height );

	//get coordinates
	coords = GetCoordinates( texX, texY, width, height, tex->w, tex->h, mirrored );

	sprite->coordinates = coords;
	sprite->tex = tex;
	sprite->vertices = vertex;

	return sprite;
}


vertex3farr* Graphics::GetVertex(float x, float y, float z, float width, float height)
{
	vertex3farr* v;
	v = new vertex3farr();

	float halfwidth = width/2;
	float halfheight = height/2;
	float xp = x + halfwidth;
	float xm = x - halfwidth;
	float yp = y + halfheight;
	float ym = y - halfheight;

	v->lb.x = xm;
	v->lb.y = ym;

	v->lt.x = xm;
	v->lt.y = yp;

	v->rb.x = xp;
	v->rb.y = ym;

	v->rt.x = xp;
	v->rt.y = yp;

	v->z = 0.0f; //TODO: z-index groups: interface, units, map

	return v;
}

coord2farr* Graphics::GetCoordinates(float x1, float y1, float x2, float y2,
										float width, float height, short mirrored)
{
	coord2farr* c;

	c = new coord2farr();

	float cx1 = x1 / width;
	float cx2 = x2/width;  //( x1 + x2 ) / width;
	float cy1 = y1 / height;
	float cy2 = y2/height; // ( y1 + y2 ) / height;

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


void Graphics::DrawGLScene()
{
	glLoadIdentity();
	SDL_GL_SwapBuffers();
}

void Graphics::CleanGLScene()
{
	glClear( GL_COLOR_BUFFER_BIT );
}

void Graphics::AddGLTexture( Texture* texture, string name)
{
	LoadedGLTextures[name] = texture;
}

Texture* Graphics::getGLTexture( string name )
{
	map <string, Texture* >::iterator it;
	it = LoadedGLTextures.find(name);
	if( it != LoadedGLTextures.end() ){
		return it->second;
	}
	return NULL;
}

void Graphics::LoadAnimation( string name, int rows, int cols, int width, int height, int texw, int texh )
{
	//Animations.erase( Animations.find(name) );
	for( int row = 0; row < rows; ++row ){
		for( int col = 0; col < cols; ++col ){
			int cw = col *  width + col;
			int rh = row * height + row;
			Animations[name].push_back(
					GetCoordinates( cw , rh,
							cw  + width, rh  + height,
							texw, texh, 0)
				);
	   }
   }
}

/*SDL_Rect Graphics::GetSDLRect( int startx, int starty, int width, int height )
{
	SDL_Rect temp_rect;

	temp_rect.x = startx;
	temp_rect.y = starty;
	temp_rect.w = width;
	temp_rect.h = height;

	return temp_rect;
}
*/

/*
SDL_Rect* Graphics::GetAnim( string name, int num )
{
	//FIXME: Add check for correct index.
	//return &Animations[name].at(num);
	return NULL;
}
*/

coord2farr* Graphics::GetAnimation( string name, unsigned int num )
{
	//FIXME: Add check for correct index.
	if( num < Animations[name].size() ){
		return Animations[name].at(num);
	}else{
		return Animations[name].at(0);
	}
	return NULL;
}
/*
Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	*//* Здесь p - это адрес пиксела, который мы хотим получить */

	/*Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
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
		return 0;	  */ /* нe должно выполниться, но позволяет избежать предупреждений */
	/*}
}*/
/*
void Graphics::SetAlpha( SDL_Surface* sfc, int alpha){
	SDL_SetAlpha( sfc, SDL_SRCALPHA, alpha );	
} 
*/
