/*
 * Primitives.h
 *
 *  Created on: 20.03.2013
 */

#ifndef GLPRIMITIVES_H_
#define GLPRIMITIVES_H_

#include "basic_types.h"
#include "utils/list.h"
#include "graphics/GraphicsTypes.h"


namespace GLPrimitives
{
	void prepare_line( float p[4], float thikness, UINT color,
			list< VertexV2FT2FC4UI* >* lines, int alpha = 255 );

	void prepare_rectangle( float x, float y, float width, float height, UINT color,
			list< VertexV2FT2FC4UI* >* lines, int alpha = 255 );

	void prepare_circle( float x, float y, float radius, UINT color,
				list< VertexV2FT2FC4UI* >* lines, int alpha = 255 );

	void prepare_triangles( float* pts, int pts_count, UINT color,
			list< VertexV2FT2FC4UI* >* lines, int alpha = 255 );
	void prepare_triangles( float* pts, int pts_count, UINT* color,
			list< VertexV2FT2FC4UI* >* lines, int alpha = 255 );
}


#endif /* GLPRIMITIVES_H_ */
