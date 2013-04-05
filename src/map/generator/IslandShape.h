/*
 * IslandShape.h
 *
 *  Created on: 23.03.2013
 */

#ifndef ISLANDSHAPE_H_
#define ISLANDSHAPE_H_

#include "basic_types.h"
#include "3rdparty/rand31.h"

struct ShapeGenerateFunc
{
	bool (*function)( s2f, void* param );
	void* param;
	bool call( s2f p )
	{
		return function( p, param );
	}
};


struct ShapeHeightGenerateFunc
{
	float (*function)( int, int, float, float, void* param );
	void* param;
	float call( int x, int y, float d, float h )
	{
		return function( x, y, d, h, param );
	}
};

struct HeightMap
{
	int width;
	int height;
	int size;
	float max;
	float min;
	float** data;
};


enum IslandForms
{
	ifRadial = 0, ifPerlin, ifSquare
};

typedef enum IslandForms IslandForm;


namespace IslandShape
{
	/* This class has factory functions for generating islands of
	 * different shapes. The factory returns a function that takes a
	 * normalized point (x and y are -1 to +1) and returns true if the
	 * point should be on the island, and false if it should be water
	 * (lake or ocean).
	 */

	ShapeHeightGenerateFunc* defaultHeight( rand31* );

	ShapeGenerateFunc* getShape( IslandForm, rand31* );

	/* Diamond square algorithm generates cloud/plasma fractal heightmap
	 * http://en.wikipedia.org/wiki/Diamond-square_algorithm
	 * Map data will be allocated.
	 * Size will be the next power of two
	 */
	void diamondSquare( HeightMap* map, int size, ShapeHeightGenerateFunc* f );
};

typedef ShapeGenerateFunc (*GeneratorFunc)( rand31* );


#endif /* ISLANDSHAPE_H_ */
