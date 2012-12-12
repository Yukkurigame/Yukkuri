/*
 * Brush.cpp
 *
 *  Created on: 05.12.2012
 */

#include "graphics/sprite/Brush.h"
#include "Define.h" // FIXME: GL functions in define
#include <cstring>


GLBrush::GLBrush( enum primitives t, short centered ) : type(t), vertex_origin(), flags()
{
	//vertices = NULL;
	point_index = 0;
	points_count = 0;
	if( centered )
		flags |= 1;
	switch(t){
		case prQUADS:
		{
			resize_verticles(4);
			VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
			for( int i=0; i < 4; ++i ){
				s3f& vrt = arr[i].verticles;
				switch(i){
					case qcRB:
						if( centered )
							vrt = s3f(0.5, -0.5, 1.0);
						else
							vrt = s3f(1.0, 0.0, 1.0);
						break;
					case qcRT:
						if( centered )
							vrt = s3f(0.5, 0.5, 1.0);
						else
							vrt = s3f(1.0, 1.0, 1.0);
						break;
					case qcLB:
						if( centered )
							vrt = s3f(-0.5, -0.5, 1.0);
						else
							vrt = s3f(0.0, 0.0, 1.0);
						break;
					case qcLT:
						if( centered )
							vrt = s3f(-0.5, 0.5, 1.0);
						else
							vrt = s3f(0.0, 1.0, 1.0);
						break;
				}
			}
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



void GLBrush::resize_verticles( int size )
{
	VBOArray::freeSpace( point_index, points_count );
	points_count = size;
	if( points_count < 1 )
		return;
	point_index = VBOArray::getSpace( points_count );
	VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
	for( int i = 0; i < size; ++i ){
		arr->verticles = s3f();
		arr->coordinates = s2f();
		arr->color = s4ub();
		arr++;
	}
}


void GLBrush::scale( float x, float y )
{
	if( points_count < 2 )
		return;
	VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
	for( int i=0; i < points_count; ++i ){
		s3f* v = &arr[i].verticles;
		v->x = vertex_origin.x + (v->x - vertex_origin.x) * x;
		v->y = vertex_origin.y + (v->y - vertex_origin.y) * y;
	}
}


void GLBrush::set_position( float x, float y, float z )
{
	move( x - vertex_origin.x, y - vertex_origin.y, z - vertex_origin.z );
	/*s3f delta(  vertex_origin.x - x,
				vertex_origin.y - y,
				vertex_origin.z - z );
	vertex_origin -= delta;
	VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
	for( int i=0; i < points_count; ++i )
		arr[i].verticles -= delta;
	*/
}


void GLBrush::set_quad( s3f lb, s3f lt, s3f rt, s3f rb )
{
	if( points_count != 4 )
		return;
	VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
	arr[0].verticles = lt + vertex_origin;
	arr[1].verticles = lb + vertex_origin;
	arr[2].verticles = rt + vertex_origin;
	arr[3].verticles = rb + vertex_origin;
}


void GLBrush::move( float dx, float dy, float dz )
{
	s3f delta( dx, dy, dz );
	vertex_origin += delta;
	VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
	for( int i=0; i < points_count; ++i ){
		arr[i].verticles += delta;
	}
}
