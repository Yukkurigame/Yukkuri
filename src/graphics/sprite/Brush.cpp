/*
 * Brush.cpp
 *
 *  Created on: 05.12.2012
 */

#include "graphics/sprite/Brush.h"
#include "Define.h" // FIXME: GL functions in define
#include <cstring>


GLBrush::GLBrush( GLuint t ) : type(t), vertex_origin()
{
	//vertices = NULL;
	point_index = 0;
	points_count = 0;
	switch(t){
		case GL_QUADS:
		{
			resize_verticles(4);
			VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
			for( int i=0; i < 4; ++i ){
				s3f& vrt = arr[i].verticles;
				switch(i){
					case qcRT:
						vrt = s3f(1.0, 1.0, 1.0);
						break;
					case qcRB:
						vrt = s3f(1.0, -1.0, 1.0);
						break;
					case qcLT:
						vrt = s3f(-1.0, 1.0, 1.0);
						break;
					case qcLB:
						vrt = s3f(-1.0, -1.0, 1.0);
						break;
				}
			}
			// lt.y // rt.x // rt.y // rb.y
			// vertices[0].y = vertices[2].x = vertices[2].y = vertices[3].x =  1;
			// lt.x // lb.x // lb.y // rb.y
			//vertices[0].x = vertices[1].x = vertices[1].y = vertices[3].y = -1;
			break;
		}
		default:
			break;
	}
}

GLBrush::~GLBrush( )
{
	VBOArray::freeSpace( point_index, points_count );
}
