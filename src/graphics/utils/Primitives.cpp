/*
 * Primitives.cpp
 *
 *  Created on: 20.03.2013
 */

#include "graphics/utils/Primitives.h"
#include "math.h"

#define MAKE_TRIANGLES( points, points_count, color, arr )	\


void GLPrimitives::prepare_line( float p[4], float thikness, UINT color,
		list< VertexV2FT2FC4UI* >* lines, int alpha )
{
	float x1 = p[0];
	float x2 = p[2];
	float y1 = p[1];
	float y2 = p[3];

	float angle = atan2( y2 - y1, x2 - x1 );
	float sina = thikness / 2.0 * sin( angle );
	float cosa = thikness / 2.0 * cos( angle );
	float pts[12] = { x1 + sina, y1 - cosa,
					x2 + sina, y2 - cosa,
					x2 - sina, y2 + cosa,
					x2 - sina, y2 + cosa,
					x1 - sina, y1 + cosa,
					x1 + sina, y1 - cosa, };

	prepare_triangles( pts, 12, color, lines );
}

void GLPrimitives::prepare_rectangle( float x, float y, float width, float height, UINT color,
		list< VertexV2FT2FC4UI* >* lines, int alpha )
{
	float pts[12] = { x, y,
					x + width, y,
					x, y + height,
					x, y + height,
					x + width, y,
					x + width, y + height
	};

	prepare_triangles( pts, 12, color, lines );
}

void GLPrimitives::prepare_circle( float x, float y, float radius, UINT color,
		list< VertexV2FT2FC4UI* >* lines, int alpha )
{
	float rad = M_PI / 180;
	int angle_step = 10;

	for( int angle = angle_step, old_angle = 0; angle < 360; angle += angle_step, old_angle += angle_step ){
		float pts[6] = {
			x, y,
			x + (float)cos(angle*rad)*radius, y + (float)sin(angle*rad)*radius,
			x + (float)cos(old_angle*rad)*radius, y + (float)sin(old_angle*rad)*radius,
		};

		prepare_triangles( pts, 6, color, lines );
	}
}


void GLPrimitives::prepare_triangles( float* points, int pts_count, UINT color,
		list< VertexV2FT2FC4UI* >* lines, int alpha )
{
	for( int i = 0; i < pts_count; i += 2 ){
		VertexV2FT2FC4UI* point = new VertexV2FT2FC4UI;
		point->verticles.x = points[i];
		point->verticles.y = points[i + 1];
		point->color.set( color );
		point->color.a = alpha;
		lines->push_back( point );
	}
}

/*
 * This function allows to setup individual color for each point
 */
void GLPrimitives::prepare_triangles( float* points, int pts_count, UINT* color,
		list< VertexV2FT2FC4UI* >* lines, int alpha )
{
	for( int i = 0; i < pts_count; i += 2 ){
		VertexV2FT2FC4UI* point = new VertexV2FT2FC4UI;
		point->verticles.x = points[i];
		point->verticles.y = points[i + 1];
		point->color.set( color[i >> 1] );
		point->color.a = alpha;
		lines->push_back( point );
	}
}
