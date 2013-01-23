/*
 * GraphicsTypes.h
 *
 *  Created on: 27.06.2010
 */

#ifndef GRAPHICSTYPES_H_
#define GRAPHICSTYPES_H_

#include "Define.h"
#include "basic_types.h"

#include <string>
#include <cstdlib>


enum GLTextureMaps {
	gltColor = 0,
	gltNormal = 1,
	gltLast = 2
};

#define GL_TEXTURE_FROM_INDEX(i) GL_TEXTURE0 + i
#define BUFFER_OFFSET(i) ((char*)NULL + (int)(i))


struct s4ub
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
	s4ub() : r(255), g(255), b(255), a(255) {}
	s4ub(unsigned int R, unsigned int G, unsigned int B, unsigned int A) : r(R), g(G), b(B), a(A) {}
	inline void set( unsigned int cr, unsigned int cg, unsigned int cb, unsigned int ca ) {
		r = cr; g = cg; b = cb; a = ca;
	}
	inline void set( const s4ub& c ) {
			r = c.r; g = c.g; b = c.b; a = c.a;
	}
};



enum primitives {
	prPOINTS = 0, prLINES, prLINELOOP, prTRIANGLES,
	prTRIANGLESTRIP, prTRIANGLESFAN, prQUADS, prPOLYGON, prLAST
};

static const GLuint gl_methods[prLAST] = {
	GL_POINTS, GL_LINES, GL_LINE_LOOP, GL_TRIANGLES,
	GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_POLYGON
};



struct VertexV2FT2FC4UI
{
	s3f verticles;
	s2f coordinates;
	s4ub color;
	VertexV2FT2FC4UI( s3f& p, s2f& t, s4ub& c ){
		verticles = p;
		coordinates = t;
		color = c;
	}
	void set( s3f* p, s2f* t, s4ub* c ){
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
	std::string id;
	std::string image;
	s2f offset;
	rect2i abs; // Absolute position in atlas; atlas absolute size
	rect2f atlas; // Relative position in atlas; atlas size
	Texture* texture;

	TextureProxy( ) : rows(), cols(), id(), image() {}

	inline bool operator < ( const TextureProxy & t ) {
		return abs < t.abs;
	}

};

enum quad_corners {
	qcRT=0, qcRB, qcLT, qcLB, qcLAST
};


inline void init_coords( VertexV2FT2FC4UI* points, const rect2f* rect )
{
	// Textures are inverted in gl space, so invert it in render space back
	// FIXME: just make some glPerspective, lol
	for( int i=0; i < qcLAST; ++i ){
		s2f& coords = points[i].coordinates;
		switch(i){
			case qcRB:
				coords = s2f(rect->x + rect->width, rect->y + rect->height);
				break;
			case qcRT:
				coords = s2f(rect->x + rect->width, rect->y);
				break;
			case qcLB:
				coords = s2f(rect->x, rect->y + rect->height);
				break;
			case qcLT:
				coords = s2f(rect->x, rect->y);
				break;
		}
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
	TextureInfo () : rows(0), cols(0), swidth(0), sheight(0),
		twidth(0), theight(0), atlas(0), pos() {}

	void fromTextureProxy( TextureProxy* t ){
		fromTextureProxy(t, 0);
	}
	void fromTextureProxy( TextureProxy* t, GLuint a ){
		rows = ( t->rows < 1 ? 1 : t->rows );
		cols = ( t->cols < 1 ? 1 : t->cols );
		swidth = t->abs.width / cols;
		sheight = t->abs.height / rows;
		atlas = a;
		pos = t->atlas;
		twidth = pos.width / static_cast<float>(cols);
		theight = pos.height / static_cast<float>(rows);
	}
	inline void getTexturePosition( s2f& tpos, int pic ){
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
	inline void getSubTexture( int pic, VertexV2FT2FC4UI* rect, int count ){
		int row = pic / cols;
		int col = pic - row * cols;
		getSubTexture( col, row, rect, count );
	}
	inline void getSubTexture( int col, int row, VertexV2FT2FC4UI* rect, int count ){
		if( count < qcLAST )
			return;
		rect2f s(0.0, 0.0, 1.0, 1.0);
		if( cols > 0 || rows > 0 ){
			col %= cols;
			row %= rows;
			s.x = pos.x + col * twidth;
			s.y = pos.y + row * theight;
			s.width = twidth;
			s.height = theight;
		}
		init_coords( rect, &s );
	}
};



#define QUAD_TRIANGLES_POINTS 2

struct VBOStructureHandle
{
	GLuint* indexes;
	int count;
	enum primitives type;
	GLuint method;
	GLuint shader;
	list< GLuint > textures;
	UINT material;

	VBOStructureHandle( enum primitives t, list< GLuint >* tex, UINT mat ) : textures(tex) {
		type = t;
		method = gl_methods[type];
		//atlas = tex;
		//normals = n;
		//shader = shd;
		indexes = NULL;
		count = 0;
		//next = NULL;
		material = mat;
	}
	~VBOStructureHandle( ){
		if( indexes )
			free(indexes);
	}

	// FIXME: QUADS TO TRIANGLES CONVERTION
	void set_indexes( int first, int c ){
		int quad_to_triangles = 0;
		if( type == prQUADS )
			quad_to_triangles = QUAD_TRIANGLES_POINTS;
		int new_count = count + c + quad_to_triangles;
		indexes = (GLuint*)realloc( indexes, (unsigned)sizeof(GLuint) * new_count );
		for( int i = 0; i < c; ++i ){
			indexes[i + count] = first + i;
		}
		if( quad_to_triangles ){
			indexes[c + count] = indexes[c + count - 2];
			indexes[c + count + 1] = indexes[c + count - 3];
		}
		count = new_count;
	}
};






#endif /* GRAPHICSTYPES_H_ */
