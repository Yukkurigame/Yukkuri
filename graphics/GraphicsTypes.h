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
};

struct Texture
{
	GLuint* texture;
	int w;
	int h;
	Color clr;
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
	Color* col;
	Sprite(){
		tex = NULL;
		vertices = NULL;
		coordinates = NULL;
		col = NULL;
	}
};


#endif /* GRAPHICSTYPES_H_ */
