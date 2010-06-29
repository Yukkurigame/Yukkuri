/*
 * GraphicsTypes.h
 *
 *  Created on: 27.06.2010
 */

#ifndef GRAPHICSTYPES_H_
#define GRAPHICSTYPES_H_

#include "SDL_opengl.h"

struct Texture
{
	GLuint* texture;
	int w;
	int h;

};

struct coord2f
{
	float x;
	float y;
	coord2f() : x(), y() {}
};

struct coord2farr
{
	coord2f lt; //left-top
	coord2f lb; //left-bottom
	coord2f rt; //right-top
	coord2f rb; //right-bottom
};

struct vertex2f
{
	float x;
	float y;
	vertex2f() : x(), y() {}
};

struct vertex3farr
{
	vertex2f lt; //left-top
	vertex2f lb; //left-bottom
	vertex2f rt; //right-top
	vertex2f rb; //right-bottom
	float z;
};


struct Sprite
{
	Texture* tex;
	vertex3farr* vertices;
	coord2farr* coordinates;
};


#endif /* GRAPHICSTYPES_H_ */
