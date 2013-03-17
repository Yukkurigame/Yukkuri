/*
 * GraphicsTypes.h
 *
 *  Created on: 27.06.2010
 */

#ifndef GRAPHICSTYPES_H_
#define GRAPHICSTYPES_H_

#include "Define.h"
#include "types.h"
#include "basic_types.h"
#include "utils/list.h"

#include "string.h"
#include <cstdlib>


enum GLTextureMaps
{
	gltColor = 0, gltNormal = 1, gltLast = 2
};

#define GL_TEXTURE_FROM_INDEX(i) GL_TEXTURE0 + i
#define BUFFER_OFFSET(i) ((char*)NULL + (int)(i))

struct s4ub
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
	s4ub( ) : r( 255 ), g( 255 ), b( 255 ), a( 255 ) {}
	s4ub( unsigned int hex ) :	r( ( hex >> 16 ) & 0xff ),
		g( ( ( hex >> 8 ) & 0xFF ) ), b( hex & 0xFF ) {	}
	s4ub( unsigned int R, unsigned int G, unsigned int B, unsigned int A ) :
		r( R ), g( G ), b( B ), a( A ) { }
	inline void set( const s4ub& c )
	{
		r = c.r; g = c.g; b = c.b; a = c.a;
	}
	inline void set( unsigned int hex ) {
		r = ( hex >> 16 ) & 0xff;
		g = ( hex >> 8 ) & 0xFF;
		b = hex & 0xFF;
	}
	inline void set( unsigned int cr, unsigned int cg, unsigned int cb, unsigned int ca )
	{
		r = cr; g = cg; b = cb; a = ca;
	}

};

// For colors, normalized version
struct s3fc
{
	float r;
	float g;
	float b;
	s3fc( ) : r( 1.0f ), g( 1.0f ), b( 1.0f ) { }
	s3fc( const s4ub& clr )
	{
		r = clr.r / 255.0f;
		g = clr.g / 255.0f;
		b = clr.b / 255.0f;
	}
};

static const GLuint gl_methods[7] = { GL_POINTS, GL_LINES, GL_LINE_LOOP, GL_TRIANGLES,
		GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_POLYGON };

struct VertexV2FT2FC4UI
{
	s3f verticles;
	s2f coordinates;
	s4ub color;
	VertexV2FT2FC4UI( ) : verticles(), coordinates(), color() {}
	VertexV2FT2FC4UI( const s3f& p, const s2f& t, const s4ub& c )
	{
		verticles = p;
		coordinates = t;
		color = c;
	}
	void set( s3f* p, s2f* t, s4ub* c )
	{
		verticles = *p;
		coordinates = *t;
		color = *c;
	}
};

struct Texture
{
	GLuint tex;
	GLuint normals;
	int w;
	int h;
};

// FIXME: PLEASE, CLEAN AND DESCRIBE THIS SHIT
// TODO: Describe
struct TextureProxy
{
	int rows;
	int cols;
	const char* id;
	const char* image;
	s2f offset;
	rect2i abs; // Absolute position in atlas; atlas absolute size
	rect2f atlas; // Relative position in atlas; atlas size
	Texture* texture;

	TextureProxy( ) :
			rows(), cols(), id(), image(), texture( NULL )
	{
	}

	inline bool operator <( const TextureProxy & t )
	{
		return abs < t.abs;
	}

};

enum quad_corners
{
	qcRight = 1, qcBottom = 2, qcFront = 4,
};

inline void init_coords( VertexV2FT2FC4UI* points, const rect2f* rect, UINT* indices,
		int count )
{
	for( int i = 0; i < count; ++i ){
		s2f& coords = points[i].coordinates;
		coords.x = rect->x + ( indices[i] & qcRight ? rect->width : 0 );
		coords.y = rect->y + ( indices[i] & qcBottom ? rect->height : 0 );
	}
}

struct TextureInfo
{
	int rows;
	int cols;
	int swidth; // width of one section
	int sheight; // height of one section
	float twidth; // section width in atlas coordinates
	float theight; // section height in atlas coordinates
	GLuint atlas; // atlas id
	GLuint normals; // normals id
	char* id;
	rect2f pos;
	TextureInfo( ) :
			rows( 0 ), cols( 0 ), swidth( 0 ), sheight( 0 ), twidth( 0 ), theight( 0 ), atlas(
					0 ), normals( 0 ), id( NULL ), pos()
	{
	}

	void fromTextureProxy( TextureProxy* t )
	{
		fromTextureProxy( t, 0 );
	}
	void fromTextureProxy( TextureProxy* t, GLuint a )
	{
		rows = ( t->rows < 1 ? 1 : t->rows );
		cols = ( t->cols < 1 ? 1 : t->cols );
		swidth = t->abs.width / cols;
		sheight = t->abs.height / rows;
		atlas = a;
		pos = t->atlas;
		twidth = pos.width / static_cast<float>( cols );
		theight = pos.height / static_cast<float>( rows );
	}
	inline void getTexturePosition( s2f& tpos, int pic )
	{
		int row = pic / cols;
		int col = pic - row * cols;
		if( cols < 1 && rows < 1 ){
			tpos.x = pos.x;
			tpos.y = pos.y;
		}else{
			col %= cols;
			row %= rows;
			tpos.x = pos.x + col * swidth;
			tpos.y = pos.y + row * sheight;
		}
	}
	inline void getSubTexture( int pic, VertexV2FT2FC4UI* rect, int count, UINT* indices )
	{
		int row = pic / cols;
		int col = pic - row * cols;
		getSubTexture( col, row, rect, count, indices );
	}
	inline void getSubTexture( int col, int row, VertexV2FT2FC4UI* coords, int count,
			UINT* indices )
	{
		if( count < 4 )
			return;
		rect2f s( 0.0, 0.0, 1.0, 1.0 );
		if( cols > 0 || rows > 0 ){
			col %= cols;
			row %= rows;
			s.x = pos.x + col * twidth;
			s.y = pos.y + row * theight;
			s.width = twidth;
			s.height = theight;
		}
		init_coords( coords, &s, indices, count );
	}
};

#define QUAD_TRIANGLES_POINTS 2

struct VBOStructureHandle
{
	GLuint* indexes;
	int indexes_alloc;
	int count;
	//enum primitives type;
	GLuint method;
	UINT vbo;
	list<GLuint> textures;
	UINT material;

	VBOStructureHandle( GLuint m, UINT VBO, list<GLuint>* tex, UINT mat ) :
			indexes( NULL ), indexes_alloc( 1 ), count( 0 ), method( m ), vbo( VBO ), textures(
					tex ), material( mat )
	{
	}

	~VBOStructureHandle( )
	{
		if( indexes )
			free( indexes );
	}

	void set_indexes( const UINT* face_indexes, UINT c )
	{
		int new_count = count + c;

		while( new_count >= indexes_alloc ){
			indexes_alloc <<= 1;
			indexes = (GLuint*)realloc( indexes,
					(unsigned)sizeof(GLuint) * indexes_alloc );
		}

		memcpy( &indexes[count], face_indexes, c * (UINT)sizeof(UINT) );

		count = new_count;
	}
};

#endif /* GRAPHICSTYPES_H_ */
