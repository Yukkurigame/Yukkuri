/*
 * GraphicsTypes.h
 *
 *  Created on: 27.06.2010
 */

#ifndef GRAPHICSTYPES_H_
#define GRAPHICSTYPES_H_

#include "SDL/SDL_opengl.h"

#include <string>
#include <cstdlib>


struct s2i
{
	signed int x;
	signed int y;
	s2i() : x(), y() {}
};


struct s2f
{
	float x;
	float y;
	s2f() : x(), y() {}
};

struct s3f
{
	float x;
	float y;
	float z;
	s3f() : x(), y(), z() {}
	s3f operator-(const s3f& b){
		s3f result = *this;
		result.x -= b.x;
		result.y -= b.y;
		result.z -= b.z;
		return result;
	}
};


struct s4ub
{
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
	s4ub() : r(), g(), b(), a() {}
};


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


struct rect2f
{
	float x;
	float y;
	float width;
	float height;
	rect2f () : x(), y(), width(), height() {}
};

struct rect2i
{
	int x;
	int y;
	int width;
	int height;
	rect2i () : x(), y(), width(), height() {}
	inline bool operator < ( const rect2i& r ) {
		return  r.width * r.height < width * height;
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

	inline bool operator < ( const TextureProxy& t ) {
		return abs < t.abs;
	}

};


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
	inline coord2farr getSubTexture( int pic ){
		int row = pic / cols;
		int col = pic - row * cols;
		return getSubTexture( col, row );
	}
	coord2farr getSubTexture(int col, int row){
		coord2farr rect;
		if( cols < 1 && rows < 1 ){
			rect.lb.x = rect.lt.x = 0;
			rect.lb.y = rect.rb.y = 0;
			rect.rb.x = rect.rt.x = 1.0;
			rect.lt.y = rect.rt.y = 1.0;
		}else{
			col %= cols;
			row %= rows;
			float x = pos.x + col * twidth;
			float y = pos.y + row * theight;
			rect.lb.x = rect.lt.x = x;
			rect.lb.y = rect.rb.y = y;
			rect.rb.x = rect.rt.x = x + twidth;
			rect.lt.y = rect.rt.y = y + theight;
		}
		return rect;
	}
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


struct VBOStructureHandle
{
	int start;
	int count;
	GLuint atlas;
	GLuint shader;
	VBOStructureHandle* next;
	VBOStructureHandle(int tex, int shd, int s){
		atlas = tex;
		shader = shd;
		start = s;
		count = 1;
		next = NULL;
	}
};


struct Sprite
{
	unsigned int texid;
	unsigned int picture;
	GLuint atlas;		// Texture atlas, same as in tex probably
	unsigned int flags; // 1 - visible
						// 2 - centred
						// 4 - fixed
	GLuint shader;
	rect2f rect;
	TextureInfo* tex;
	vertex3farr vertices;
	coord2farr coordinates;
	color4u clr;

	inline unsigned char isVisible()	{ return flags & 1; }
	inline void setVisible()			{ flags |= 1; }
	inline void clearVisible()			{ flags &= ~1; }

	inline unsigned char isCentered()	{ return flags & 2; }
	inline void setCentered()			{ flags |= 2; }
	inline void clearCentered()			{ flags &= ~2; }

	inline unsigned char isFixed()		{ return flags & 4; }
		   void setFixed();				// Needs shader placing
	inline void clearFixed()			{ flags &= ~4; shader = 0; }


	Sprite(){
		tex = NULL;
		picture = atlas = texid = 0;
		flags = 0b1; // visible only
		shader = 0;
	}

	void setPicture( int pic ){
		picture = pic;
		if( texid && tex ){
			coordinates = tex->getSubTexture(pic);
		}else{
			coordinates.lb.x = coordinates.lt.x = 0;
			coordinates.lb.y = coordinates.rb.y = 0;
			coordinates.rb.x = coordinates.rt.x = 1.0;
			coordinates.lt.y = coordinates.rt.y = 1.0;
		}
	}

	void resize( float w, float h ){
		if( w >= 0 ){
			vertices.rb.x = vertices.rt.x = vertices.lb.x + w;
			rect.width = w;
		}
		if( h >= 0 ){
			vertices.rt.y = vertices.lt.y = vertices.lb.y + h;
			rect.height = h;
		}
	}

	void move( float dx, float dy, float dz ){
		// move x
		vertices.lb.x = (vertices.lt.x += dx);
		vertices.rb.x = (vertices.rt.x += dx);
		// move y
		vertices.lb.y = (vertices.rb.y += dy);
		vertices.lt.y =	(vertices.rt.y += dy);
		// move z
		vertices.lb.z = vertices.lt.z = vertices.rt.z = (vertices.rb.z += dz);
	}

	void setPosition( float x, float y ){
		float width = vertices.rb.x - vertices.lb.x;
		float height = vertices.rt.y - vertices.lb.y; // FIXME: lb is rb?
		rect.x = x;
		rect.y = y;
		if( isCentered() ){
			float halfwidth = width/2;
			float halfheight = height/2;
			vertices.lb.x = vertices.lt.x = x - halfwidth;
			vertices.lb.y = vertices.rb.y = y - halfheight;
			vertices.rb.x = vertices.rt.x = x + halfwidth;
			vertices.lt.y = vertices.rt.y = y + halfheight;
		}else{
			vertices.lb.x = vertices.lt.x = x;
			vertices.lb.y = vertices.rb.y = y;
			vertices.rb.x = vertices.rt.x = x + width;
			vertices.lt.y = vertices.rt.y = y + height;
		}
	}

	void setPosition( float x, float y, float z ){
		setPosition( x, y );
		vertices.lb.z = vertices.lt.z = vertices.rt.z = vertices.rb.z = z;
	}

	void toggleVisibility( ){
		if( isVisible() )
			clearVisible();
		else
			setVisible();
	}

};


#endif /* GRAPHICSTYPES_H_ */
