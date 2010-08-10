/*
 * GraphicsTypes.h
 *
 *  Created on: 27.06.2010
 */

#ifndef GRAPHICSTYPES_H_
#define GRAPHICSTYPES_H_

#include "SDL/SDL_opengl.h"

struct Color
{
	unsigned int r;
	unsigned int b;
	unsigned g;
	unsigned a;
	Color(): r(255), b(255), g(255), a(255) {};
	Color(unsigned int r, unsigned int b, unsigned int g): r(r), b(b), g(g), a(255) {};
	Color(unsigned int r, unsigned int b, unsigned int g, unsigned int a): r(r), b(b), g(g), a(a) {};
	bool isMax( ){
		if( r != 255 || g != 255 || b != 255 ) return false;
		return true;
	}
	void set( int c ) { r = g = b = c; }
	void set( unsigned int c ) { r = g = b = c; }
	void set( unsigned int cr, unsigned int cg, unsigned int cb ) { r = cr; g = cg; b = cb; }
	void set( unsigned int cr, unsigned int cg, unsigned int cb, unsigned int ca ) { r = cr; g = cg; b = cb; a = ca; }
	void set( Color* c ) { r = c->r; g = c->g; b = c->b; a = c->a; }
};

struct Texture
{
	GLuint* texture;
	int w;
	int h;
	Color clr;
	Texture() { texture = NULL; w = 0; h = 0; }
	Texture( Texture* o ){
		texture = o->texture;
		w = o->w;
		h = o->h;
		clr = o->clr;
	}

};

struct s2f
{
	float x;
	float y;
	s2f() : x(), y() {}
};

struct coord2farr
{
	s2f lt; //left-top
	s2f lb; //left-bottom
	s2f rt; //right-top
	s2f rb; //right-bottom
};

struct vertex3farr
{
	s2f lt; //left-top
	s2f lb; //left-bottom
	s2f rt; //right-top
	s2f rb; //right-bottom
	float z;
};


struct Sprite
{
	Texture* tex;
	vertex3farr* vertices;
	coord2farr* coordinates;
	Color* clr;
	Sprite(){
		tex = NULL;
		vertices = NULL;
		coordinates = NULL;
		clr = NULL;
	}
	void resize( float w, float h ){
		if( w >= 0 )
			vertices->rb.x = vertices->rt.x = vertices->lb.x + w;
		if( h >= 0 )
			vertices->rt.y = vertices->lt.y = vertices->lb.y + h;
	}
	void setPosition( float x, float y ){
		float width = vertices->rb.x - vertices->lb.x;
		float height = vertices->rt.y - vertices->lb.y;
		vertices->lb.x = vertices->lt.x = x;
		vertices->lb.y = vertices->rb.y = y;
		vertices->rb.x = vertices->rt.x = x + width;
		vertices->lt.y = vertices->rt.y = y + height;
	}
};


#endif /* GRAPHICSTYPES_H_ */
