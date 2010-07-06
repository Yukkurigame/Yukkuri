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

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glTranslatef(0.0f, 0.0f, 6.0f);

	glClearColor( 0.25, 0.43, 0.0, -1.0 );

	glClearDepth( 10.0f );

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_BLEND);


	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glOrtho(0.0, wwidth, wheight, 0.0, -1.0, 1.0);

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
			debug(3, name + " not loaded");
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
			debug(3, name + " is not truecolor..  this will probably break");
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

void Graphics::FreeGLTexture( Texture* tex )
{
	//FIXME: segfault
	glDeleteTextures( 1, tex->texture );
	//delete tex;
}

void Graphics::DrawGLTexture( Texture* tex, vertex3farr* vertices, coord2farr* coordinates, Color* col)
{
	if(tex){
		glBindTexture( GL_TEXTURE_2D, *tex->texture );
	}else{
		glBindTexture( GL_TEXTURE_2D, 0 );
	}
	//Sprite color
	glColor4ub(col->r,col->g,col->b,col->a);
	if(coordinates){
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
	}else{
		glBegin( GL_QUADS ); //draw rectangle;
			glVertex3f( vertices->lb.x, vertices->lb.y, vertices->z ); //Bottom-left
			glVertex3f( vertices->rb.x, vertices->rb.y, vertices->z ); //Bottom-right
			glVertex3f( vertices->rt.x, vertices->rt.y, vertices->z ); //Top-right
			glVertex3f( vertices->lt.x, vertices->lt.y, vertices->z ); //Top-left
		glEnd();
	}
}

void Graphics::LoadAllTTFonts( int size )
{
	debug(3, "Loading fonts\n");
	string dirname = "data/shared/"; //TODO: add FONTPATH
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
    	debug(3, "\tFAIL\n");
    	debug(3, "Bad directory.\n");
        return;
    }
    //pdbg(3, "Done.\n");
    //FIXME: debug print
    char dbg[38];
    sprintf(dbg, "Loaded %d from %d font files.\n", success, files);
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

void Graphics::PrintText( string fontname, int size, float x, float y, float z, const int* color, string text)
{
	font_data* font = GetFont(fontname, size);
	if(!font){
		debug( 3,"Font " + fontname + " not found.\n" );
		return;
	}
	PrintText( *font, x, y, z, size, color, text.c_str() );
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

/* This function creating sprite structure with texture.
 * x, y, z - right top coordinates;
 * texX, texY - right top coordinates of texture rectangle;
 * width, height - width and height of sprite/texture rectangle;
 * tex - texture structure;
 * mirrored - rotates the image on the x axis;
 * centered - origin at the center of the object if not 0;
 * return Sprite*
 */
//TODO: идеально было бы написать миллион конструкторов, чтобы можно было создавать с дефолтными параметрами.
Sprite* Graphics::CreateGLSprite( float x, float y, float z, float texX, float texY,
								float width, float height, Texture* tex, short mirrored, short centered)
{
	Sprite* sprite;
	vertex3farr* vertex;
	coord2farr* coords;

	sprite = new Sprite();

	//get vertex
	vertex = GetVertex( x, y, z, width, height, centered );

	if(tex){ //no image - no rectangle;
		//get coordinates
		coords = GetCoordinates( texX, texY, width, height, tex->w, tex->h, mirrored);

		sprite->tex = tex;
		sprite->coordinates = coords;
		sprite->col = &sprite->tex->clr;
	}else{
		sprite->tex = NULL;
		sprite->coordinates = NULL;
		sprite->col = new Color( );
	}

	sprite->vertices = vertex;

	return sprite;
}

void Graphics::FreeGLSprite( Sprite* spr )
{
	//FIXME: it's memory rain
	if(!spr->tex || spr->col != &spr->tex->clr )
		delete spr->col;
	FreeGLTexture( spr->tex );
	delete spr->vertices;
	delete spr->coordinates;
	delete spr;
}

coord2farr* Graphics::GetCoordinates(float x1, float y1, float x2, float y2,
										float width, float height, short mirrored)
{
	coord2farr* c;

	c = new coord2farr();

	float cx1 = x1 / width;
	float cx2 = ( x1 + x2 ) / width; //x2/width;
	float cy1 = y1 / height;
	float cy2 = ( y1 + y2 ) / height; //y2/height;

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

vertex3farr* Graphics::GetVertex(float x, float y, float z, float width, float height, short centered)
{
	vertex3farr* v;
	v = new vertex3farr();

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

	return v;
}

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

void Graphics::DrawGLScene()
{
	glLoadIdentity();
	SDL_GL_SwapBuffers();
}

void Graphics::CleanGLScene()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
}

bool Graphics::SaveScreenshot(const char *FileName)
{
	//No screenshot;
	return false;
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
    	debug(3, "Could not load " + filename + "\n" );
        return NULL;
    }

    if( ( optimizedImage->w & (optimizedImage->w - 1) ) != 0 ){
    	debug(3, filename + " width is not a power of 2");
    	return NULL;
    }
    if( (optimizedImage->h & (optimizedImage->h - 1) ) != 0 ){
    	debug(3, filename + " height is not a power of 2");
    	return NULL;
    }

    return optimizedImage;
}

font_data* Graphics::GetFont( string name, int size  )
{
	//Как-то коряво получилось.
	if( LoadedFonts.count(name) > 0 ){
		if( LoadedFonts[name].count(size) ){
			return LoadedFonts[name][size];
		}else{
			if( LoadTTFont( "data/shared/", name, size ) ) //TODO: add FONTPATH
				return LoadedFonts[name][size];
		}
	//map <string, font_data* >::iterator it;
	//it = LoadedFonts.find(name);
	//if( it != LoadedFonts.end() ){
		//return it->second;
	//}
	}else{
		if( LoadTTFont( "data/shared/", name, size ) ) //TODO: add FONTPATH
			return LoadedFonts[name][size];
	}
	return NULL;
}

void Graphics::PrintText(const font_data &ft_font, float x, float y, float z, int size, const int* color, const char *str)
{

	if( str == NULL )
		return;

	//Костыль по-моему
	//float multipler = size/LOADEDFONTSIZE;

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
	glColor3ub(color[0],color[1],color[2]);
	glScalef(1,-1,1); //glScalef(multipler,-1*multipler,1); //Reversed text. wtf?
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glListBase(font);

	float modelview_matrix[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	for( unsigned int i=0; i<lines.size(); ++i) {
		glPushMatrix();
		glLoadIdentity();
		glTranslatef( x, y - h * i, z );
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

	//glScalef(1/multipler,-1/multipler,1); //Reversed text. wtf?
	glScalef(1,-1,1); //Reversed text. wtf?

	glPopAttrib();

	//pop_projection_matrix();

}
