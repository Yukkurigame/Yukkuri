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


struct s2f
{
	float x;
	float y;
	s2f() : x(), y() {}
};


struct s4u
{
	unsigned int r;
	unsigned int g;
	unsigned int b;
	unsigned int a;
	s4u() : r(), g(), b(), a() {}
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
	s2f lt; //left-top
	s2f lb; //left-bottom
	s2f rt; //right-top
	s2f rb; //right-bottom
	float z;
	vertex3farr(): z() {};
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


struct rect2i
{
	int x;
	int y;
	int width;
	int height;
};


struct rect2f
{
	float x;
	float y;
	float width;
	float height;

	rect2f () {}

	rect2f ( rect2i* r ){
		x = r->x;
		y = r->y;
		width = r->width;
		height = r->height;
	}
};


struct TextureS
{
	int width;
	int height;
	int offsetx;
	int offsety;
	int rows;
	int cols;
	int atlasX;
	int atlasY;
	std::string id;
	std::string image;
	rect2f atlas;
	GLuint* texture;

	TextureS( ){}

	TextureS( std::string i, std::string n, std::string img, int w, int h, int ox, int oy, int r, int c ){
		id = i;
		image = img;
		width = w;
		height = h;
		offsetx = ox;
		offsety = oy;
		rows = r;
		cols = c;
	}

	bool operator < ( TextureS t ) {
		return t.width * t.height < height * width;
	}

};


struct TextureInfo
{
	int rows;
	int cols;
	int swidth; // width of one section
	int sheight; // height of one section
	GLuint* atlas;
	const char* id;
	rect2f pos;
	TextureInfo () {
		atlas = NULL;
	}
	void fromTextureS( TextureS* t ){
		fromTextureS(t, NULL);
	}
	void fromTextureS( TextureS* t, GLuint* a ){
		rows = ( t->rows < 1 ? 1 : t->rows );
		cols = ( t->cols < 1 ? 1 : t->cols );
		swidth = t->width / cols;
		sheight = t->height / rows;
		atlas = a;
		id = t->id.c_str();
		pos = t->atlas;
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
			int x = pos.x + col * swidth;
			int y = pos.y + row * sheight;
			rect.lb.x = rect.lt.x = x;
			rect.lb.y = rect.rb.y = y;
			rect.rb.x = rect.rt.x = x + swidth;
			rect.lt.y = rect.rt.y = y + sheight;
		}
		return rect;
	}
};


struct VertexV2FT2FC4UI
{
	s2f verticles;
	s2f coordinates;
	s4u color;
	VertexV2FT2FC4UI(s2f p, s2f t, color4u* c){
		verticles = p;
		coordinates = t;
		color.r = c->r;
		color.g = c->g;
		color.b = c->b;
		color.a = c->a;
	}
};


struct VBOStructureHandle
{
	TextureInfo* texture;
	int shaders;
	int number;
	VBOStructureHandle* next;
	VBOStructureHandle(TextureInfo* a, int s, int num, VBOStructureHandle* nxt){
		texture = a;
		shaders = s;
		number = num;
		next = nxt;
	}
};


struct Sprite
{
	int col;
	int row;
	float width;
	float height;
	float posx;
	float posy;
	bool visible;
	bool centered;
	bool fixed;
	TextureInfo* tex;
	vertex3farr* vertices;
	color4u* clr;


	Sprite(){
		tex = NULL;
		vertices = NULL;
		clr = NULL;
		col = row = posx = posy = width = height = 0;
		fixed = visible = true;
		centered = false;
	}

	coord2farr getTextureCoordinates(){
		if( tex != NULL )
			return tex->getSubTexture(col, row);
		coord2farr rect;
		rect.lb.x = rect.lt.x = 0;
		rect.lb.y = rect.rb.y = 0;
		rect.rb.x = rect.rt.x = 1.0;
		rect.lt.y = rect.rt.y = 1.0;
		return rect;
	}

	void resize( float w, float h ){
		if( !vertices ) return;
		if( w >= 0 ){
			vertices->rb.x = vertices->rt.x = vertices->lb.x + w;
			width = w;
		}
		if( h >= 0 ){
			vertices->rt.y = vertices->lt.y = vertices->lb.y + h;
			height = h;
		}
	}
	void setPosition( float x, float y ){
		if( !vertices ) return;
		float width = vertices->rb.x - vertices->lb.x;
		float height = vertices->rt.y - vertices->lb.y; // FIXME: lb is rb?
		posx = x;
		posy = y;
		if(centered){
			float halfwidth = width/2;
			float halfheight = height/2;
			vertices->lb.x = vertices->lt.x = x - halfwidth;
			vertices->lb.y = vertices->rb.y = y - halfheight;
			vertices->rb.x = vertices->rt.x = x + halfwidth;
			vertices->lt.y = vertices->rt.y = y + halfheight;
		}else{
			vertices->lb.x = vertices->lt.x = x;
			vertices->lb.y = vertices->rb.y = y;
			vertices->rb.x = vertices->rt.x = x + width;
			vertices->lt.y = vertices->rt.y = y + height;
		}
	}
	void setPosition( float x, float y, float z ){
		if( !vertices ) return;
		setPosition( x, y );
		vertices->z = z;
	}
	void toggleVisibility( ){
		if( visible )
			visible = false;
		else
			visible = true;
	}

	inline bool operator < ( Sprite* s2 ){
		//add atlas check
		if( vertices->z == s2->vertices->z ){
			if( posy == s2->posy )
				return ( posx > s2->posx );
			return ( posy > s2->posy );
		}
		return ( vertices->z < s2->vertices->z );
	}
};


#endif /* GRAPHICSTYPES_H_ */
