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


struct s4ub
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
	s4ub() : r(255), g(255), b(255), a(255) {}
};



enum primitives {
	prPOINTS = 0, prLINES, prLINELOOP, prTRIANGLES,
	prTRIANGLESTRIP, prQUADS, prPOLYGON, prLAST
};

static const GLuint gl_methods[prLAST] = {
	GL_POINTS, GL_LINES, GL_LINE_LOOP, GL_TRIANGLES,
	GL_TRIANGLE_STRIP, GL_TRIANGLES, GL_POLYGON
};

/*

struct coord2farr
{
	s2f lt; //left-top
	s2f lb; //left-bottom
	s2f rt; //right-top
	s2f rb; //right-bottom

	// Calculated by top side only
	float width(){ return abs(rt.x - lt.x); }
	// Calculated by right side only
	float height(){ return abs(rt.y - rb.y); }
	coord2farr(){
		lb.x = lt.x = 0.0;
		lb.y = rb.y = 0.0;
		rb.x = rt.x = 1.0;
		lt.y = rt.y = 1.0;
	}
};


struct vertex3farr
{
	s3f lt; //left-top
	s3f lb; //left-bottom
	s3f rt; //right-top
	s3f rb; //right-bottom
	vertex3farr() {};
};

*/

struct color4u
{
	unsigned int r;
	unsigned int b;
	unsigned int g;
	unsigned int a;
	color4u(): r(255), b(255), g(255), a(255) {};
	color4u(unsigned int r, unsigned int b, unsigned int g): r(r), b(b), g(g), a(255) {};
	color4u(unsigned int r, unsigned int b, unsigned int g, unsigned int a): r(r), b(b), g(g), a(a) {};
	bool isMax( ){
		if( r != 255 || g != 255 || b != 255 ) return false;
		return true;
	}
	void set( int c ) { r = g = b = c; }
	void set( unsigned int c ) { r = g = b = c; }
	void set( unsigned int cr, unsigned int cg, unsigned int cb ) { r = cr; g = cg; b = cb; }
	void set( unsigned int cr, unsigned int cg, unsigned int cb, unsigned int ca ) { r = cr; g = cg; b = cb; a = ca; }
	void set( color4u* c ) { if( !c ) return; r = c->r; g = c->g; b = c->b; a = c->a; }
};


struct VertexV2FT2FC4UI
{
	s3f verticles;
	s2f coordinates;
	s4ub color;
	VertexV2FT2FC4UI(s3f p, s2f t, color4u* c){
		verticles = p;
		coordinates = t;
		color.r = c->r;
		color.g = c->g;
		color.b = c->b;
		color.a = c->a;
	}
	void set( s3f* p, s2f* t, color4u* c ){
		verticles = *p;
		coordinates = *t;
		color.r = c->r;
		color.g = c->g;
		color.b = c->b;
		color.a = c->a;
	}
};


struct Texture
{
	GLuint tex;
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
/*	inline coord2farr getSubTexture( int pic ){
		coord2farr rect;
		getSubTexture( pic, rect );
		return rect;
	}
*/
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
		/*
		for( int i=0; i < qcLAST; ++i ){
			s2f& coords = rect[i].coordinates;
			switch(i){
				case qcRT:
					coords = s2f(x + w, y + h);
					break;
				case qcRB:
					coords = s2f(x + w, y);
					break;
				case qcLT:
					coords = s2f(x, y + h);
					break;
				case qcLB:
					coords = s2f(x, y);
					break;
			}
		}*/
		/*
		rect.lb.x = rect.lt.x = x;
		rect.lb.y = rect.rb.y = y;
		rect.rb.x = rect.rt.x = x + twidth;
		rect.lt.y = rect.rt.y = y + theight;
		*/

	}
};


#define QUAD_TRIANGLES_POINTS 2

struct VBOStructureHandle
{
	GLuint* indexes;
	int count;
	enum primitives type;
	GLuint method;
	GLuint atlas;
	GLuint shader;
	VBOStructureHandle* next;
	VBOStructureHandle( enum primitives t, int tex, int shd ){
		type = t;
		method = gl_methods[type];
		atlas = tex;
		shader = shd;
		indexes = NULL;
		count = 0;
		next = NULL;
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
