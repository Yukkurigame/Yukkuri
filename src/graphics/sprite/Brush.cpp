/*
 * Brush.cpp
 *
 *  Created on: 05.12.2012
 */

#include "graphics/sprite/Brush.h"
#include "graphics/sprite/Mesh.h"
#include "graphics/utils/VBOArray.h"


GLBrush::GLBrush( int mesh_id ) : mesh( mesh_id ), vertex_origin(), flags()
{
	point_index = 0;
	points_count = 0;
	indices_count = 0;
	indices_list = NULL;
	MeshManager::load( this );
}


GLBrush::~GLBrush( )
{
	VBOArray::freeSpace( point_index, points_count );
}


inline s3f size( VertexV2FT2FC4UI* arr, UINT count )
{
	s3f distance[2];

#define CHK( p )				\
	if( vtx.p < distance[0].p )	\
		distance[0].p = vtx.p;	\
	if( vtx.p > distance[1].p )	\
		distance[1].p = vtx.p;

	for( UINT i = 0; i < count; ++i ){
		s3f& vtx = arr[i].verticles;
		CHK(x)
		CHK(y)
		CHK(z)
	}

#undef CHK

	return distance[1] - distance[0];
}


void GLBrush::setCentered( )
{
	if( isCentered() )
		return;
	VertexV2FT2FC4UI* arr = points();
	s3f dist = size( arr, points_count );
	for( UINT i = 0; i < points_count; ++i ){
		arr[i].verticles.x -= dist.x / 2.0f;
		arr[i].verticles.y -= dist.y / 2.0f;
		arr[i].verticles.z -= dist.z / 2.0f;
	}
	flags |= 1;
}

void GLBrush::clearCentered( )
{
	if( !isCentered() )
		return;
	VertexV2FT2FC4UI* arr = points();
	s3f dist = size( arr, points_count );
	for( UINT i = 0; i < points_count; ++i ){
		arr[i].verticles.x += dist.x / 2.0f;
		arr[i].verticles.y += dist.y / 2.0f;
		arr[i].verticles.z += dist.z / 2.0f;
	}
	flags &= ~1;
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


void GLBrush::scale( const s3f* scale )
{
	if( points_count < 2 )
		return;
	VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
	for( UINT i = 0; i < points_count; ++i ){
		s3f* v = &arr[i].verticles;
		v->x = vertex_origin.x + (v->x - vertex_origin.x) * scale->x;
		v->y = vertex_origin.y + (v->y - vertex_origin.y) * scale->y;
		v->z = vertex_origin.z + (v->z - vertex_origin.z) * scale->z;
	}
}


void GLBrush::move( float dx, float dy, float dz )
{
	s3f delta( dx, dy, dz );
	vertex_origin += delta;
	VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
	for( UINT i = 0; i < points_count; ++i ){
		arr[i].verticles += delta;
	}
}


void GLBrush::set_color( const s4ub& color )
{
	VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
	for( UINT i=0; i < points_count; ++i )
		arr[i].color.set(color);
}

s4ub GLBrush::get_color(  )
{
	VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
	return arr[0].color;
}


VertexV2FT2FC4UI* GLBrush::points(  )
{
	return VBOArray::pointer( point_index );
}
