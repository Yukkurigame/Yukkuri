/*
 * IslandShape.cpp
 *
 *  Created on: 23.03.2013
 */

#include "map/generator/IslandShape.h"
#include "map/generator/fifth-party/PMPRNG.h"
#include "utils/misc.h"
#include "string.h"
#include <cmath>
#include <stdlib.h>


struct ParamsRadial
{
	double ISLAND_FACTOR;
	PM_PRNG islandRandom;
	int bumps;
	double startAngle;
	double dipAngle;
	double dipWidth;
};

struct ParamsDefaultHeight
{
	PM_PRNG randomizer;
};

namespace IslandShape
{
	const double ISLAND_FACTOR = 1.07; // 1.0 means no small islands; 2.0 leads to a lot

	void diamond( int x, int y, int d, ShapeHeightGenerateFunc* f, HeightMap* map )
	{
		float sum = 0;
		int num = 0;
		if( 0 <= x - d ){
			sum += map->data[x - d][y];
			num++;
		}
		if( x + d < map->width ){
			sum += map->data[x + d][y];
			num++;
		}
		if( 0 <= y - d ){
			sum += map->data[x][y - d];
			num++;
		}
		if( y + d < map->height ){
			sum += map->data[x][y + d];
			num++;
		}
		map->data[x][y] = f->call( x, y, d, sum/num );
	}

	void square( int x, int y, int d, ShapeHeightGenerateFunc* f, HeightMap* map )
	{
		float sum = 0;
		int num = 0;
		if( 0 <= x - d ){
			if( 0 <= y - d ){
				sum += map->data[x - d][y - d];
				num++;
			}
			if( y + d < map->height ){
				sum += map->data[x - d][y + d];
				num++;
			}
		}
		if( x + d < map->width ){
			if( 0 <= y - d ){
				sum += map->data[x + d][y - d];
				num++;
			}
			if( y + d < map->height ){
				sum += map->data[x + d][y + d];
				num++;
			}
		}
		map->data[x][y] = f->call( x, y, d, sum / num );
	}
}

float height_default( int x, int y, float d, float h, void* param  )
{
	return h + ((ParamsDefaultHeight*)param)->randomizer.nextDoubleRange(-0.5, 0.5) * d;
}


void IslandShape::diamondSquare( HeightMap* map, int size, ShapeHeightGenerateFunc* f )
{
	int d = next_p2(size);
	map->size = size;
    map->width = map->height = d;
    map->data = new float*[d];
    for(int i = 0; i < d; ++i)
    	map->data[i] = new float[d];

    // seed four corners
    map->data[0][0] = f->call(0, 0, d, 0);
    map->data[0][d-1] = f->call(0, d, d, 0);
    map->data[d-1][0] = f->call(d, 0, d, 0);
    map->data[d-1][d-1] = f->call(d, d, d, 0);

    d = d/2;
    // perform square and diamond steps
    while( 1 <= d ){
    	for( int x = d; x < map->width; x += 2 * d ){
    		for( int y = d; y < map->height; y += 2 * d )
                square(x, y, d, f, map);
    	}
		for( int x = d; x < map->width; x += 2 * d ){
			for( int y = 0; y < map->height - 1; y += 2 * d )
				diamond( x, y, d, f, map );
		}
		for( int x = 0; x < map->width - 1; x += 2 * d ){
			for( int y = d; y < map->height; y += 2 * d )
				diamond( x, y, d, f, map );
		}
		d = d/2;
    }

    // Find minimum & maximum;
    float max_height = 1;
    float min_height = 0;
    for( int x = 0; x < map->width; x++ )
    	for( int y = 0; y < map->height; y++ ){
    		float d = map->data[x][y];
    		if( d > max_height )
    			max_height = d;
    		else if( d < min_height )
    			min_height = d;
    	}

    map->max = max_height;
    map->min = min_height;
}


ShapeHeightGenerateFunc* IslandShape::defaultHeight( int seed )
{
	ShapeHeightGenerateFunc* func = new ShapeHeightGenerateFunc();
	ParamsDefaultHeight* p = new ParamsDefaultHeight();
	p->randomizer.seed = seed;
	func->function = height_default;
	func->param = p;
	return func;
}



bool radial_shape( s2f q, void* prm )
{
	ParamsRadial* param = reinterpret_cast< ParamsRadial* >( prm );
	double angle = atan2( q.y, q.x );
	double p_length = sqrt( q.x * q.x + q.y * q.y );
	double length = 0.5 * ( max( abs( q.x ), abs( q.y ) ) + p_length );

	double r1 = 0.5 + 0.40 * sin( param->startAngle + param->bumps * angle
									+ cos( ( param->bumps + 3 ) * angle ) );
	double r2 = 0.7 - 0.20 * sin( param->startAngle + param->bumps * angle
									- sin( ( param->bumps + 2 ) * angle ) );
	if( abs( angle - param->dipAngle ) < param->dipWidth
			|| abs( angle - param->dipAngle + 2 * M_PI ) < param->dipWidth
			|| abs( angle - param->dipAngle - 2 * M_PI ) < param->dipWidth ){
		r1 = r2 = 0.2;
	}
	return ( length < r1 || ( length > r1 * param->ISLAND_FACTOR && length < r2 ) );
}


ShapeGenerateFunc* IslandShape::makeRadial( int seed )
{
	ShapeGenerateFunc* func = new ShapeGenerateFunc();
	ParamsRadial* p = new ParamsRadial();
	p->ISLAND_FACTOR = ISLAND_FACTOR;
	p->islandRandom.seed = seed;
	p->bumps = p->islandRandom.nextIntRange( 1, 6 );
	p->startAngle = p->islandRandom.nextDoubleRange( 0, 2 * M_PI );
	p->dipAngle = p->islandRandom.nextDoubleRange( 0, 2 * M_PI );
	p->dipWidth = p->islandRandom.nextDoubleRange( 0.2, 0.7 );

	func->function = &radial_shape;
	func->param = p;

	return func;
}

bool perlin_shape( s2f q, void* prm )
{
	//var c:Number = (perlin.getPixel(int((q.x+1)*128), int((q.y+1)*128)) & 0xff) / 255.0;
	//	return c > (0.3+0.3*q.length*q.length);
	return true;
}

ShapeGenerateFunc* IslandShape::makePerlin( int seed )
{
	ShapeGenerateFunc* func = new ShapeGenerateFunc();
	func->function = &perlin_shape;
	//var
	//perlin: BitmapData = new BitmapData( 256, 256 );
	//perlin.perlinNoise( 64, 64, 8, seed, false, true );
	//
	return func;
}

bool square_shape( s2f q, void* prm )
{
	return true;
}

ShapeGenerateFunc* IslandShape::makeSquare( int seed )
{
	ShapeGenerateFunc* func = new ShapeGenerateFunc();
	func->function = &square_shape;
	return func;
}

bool blob_shape( s2f q, void* prm )
{
	bool eye1 = sqrt(
			( q.x - 0.2 ) * ( q.x - 0.2 ) + ( q.y / 2 + 0.2 ) * ( q.y / 2 + 0.2 ) )
			< 0.05;
	bool eye2 = sqrt(
			( q.x + 0.2 ) * ( q.x + 0.2 ) + ( q.y / 2 + 0.2 ) * ( q.y / 2 + 0.2 ) )
			< 0.05;
	bool body = sqrt( q.x * q.x + q.y * q.y ) < 0.8 - 0.18 * sin( 5 * atan2( q.y, q.x ) );
	return body && !eye1 && !eye2;
}

ShapeGenerateFunc* IslandShape::makeBlob( int seed )
{
	ShapeGenerateFunc* func = new ShapeGenerateFunc();
	func->function = &blob_shape;
	return func;
}
