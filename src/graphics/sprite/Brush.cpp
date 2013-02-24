/*
 * Brush.cpp
 *
 *  Created on: 05.12.2012
 */

#include "graphics/sprite/Brush.h"
#include "graphics/sprite/Mesh.h"
#include "graphics/utils/VBOArray.h"
#include "graphics/Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


GLBrush::GLBrush(  ) : mesh( -1 ), method(GL_TRIANGLES), vertex_origin(), flags()
{
	point_index = 0;
	points_count = 0;
	indices_count = 0;
	indices_list = NULL;
	vertex_points = NULL;
}


GLBrush::~GLBrush( )
{
	VBOArray::freeSpace( point_index, points_count );
}


short GLBrush::init( int mesh_id )
{
	mesh = mesh_id;
	return MeshManager::load( this );
}


short GLBrush::init( const char* name )
{
	int id = MeshManager::get( name );
	return init(id);
}


inline s3f size( const s3f* arr, UINT count )
{
	s3f distance[2];

#define CHK( p )					\
	if( vtx->p < distance[0].p )	\
		distance[0].p = vtx->p;		\
	if( vtx->p > distance[1].p )	\
		distance[1].p = vtx->p;

	for( UINT i = 0; i < count; ++i ){
		const s3f* vtx = &arr[i];
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
	s3f dist = size( vertex_points, points_count );
	for( UINT i = 0; i < points_count; ++i ){
		vertex_points[i].x -= dist.x / 2.0f;
		vertex_points[i].y -= dist.y / 2.0f;
		vertex_points[i].z -= dist.z / 2.0f;
	}

	/*VertexV2FT2FC4UI* arr = points();
	s3f dist = size( arr, points_count );
	for( UINT i = 0; i < points_count; ++i ){
		arr[i].verticles.x -= dist.x / 2.0f;
		arr[i].verticles.y -= dist.y / 2.0f;
		arr[i].verticles.z -= dist.z / 2.0f;
	}*/

	flags |= 1;
}

void GLBrush::clearCentered( )
{
	if( !isCentered() )
		return;

	s3f dist = size( vertex_points, points_count );
	for( UINT i = 0; i < points_count; ++i ){
		vertex_points[i].x += dist.x / 2.0f;
		vertex_points[i].y += dist.y / 2.0f;
		vertex_points[i].z += dist.z / 2.0f;
	}

	/*VertexV2FT2FC4UI* arr = points();
	s3f dist = size( arr, points_count );
	for( UINT i = 0; i < points_count; ++i ){
		arr[i].verticles.x += dist.x / 2.0f;
		arr[i].verticles.y += dist.y / 2.0f;
		arr[i].verticles.z += dist.z / 2.0f;
	}
	*/
	update_points();
	flags &= ~1;
}

void GLBrush::resize_verticles( int size )
{
	VBOArray::freeSpace( point_index, points_count );
	if( vertex_points )
		delete[] vertex_points, vertex_points = NULL;

	points_count = size;
	if( points_count < 1 )
		return;
	vertex_points = new s3f[points_count];

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
	for( UINT i = 0; i < points_count; ++i ){
		s3f* v = &vertex_points[i];
		v->x = vertex_origin.x + (v->x - vertex_origin.x) * scale->x;
		v->y = vertex_origin.y + (v->y - vertex_origin.y) * scale->y;
		v->z = vertex_origin.z + (v->z - vertex_origin.z) * scale->z;
	}

	update_points();
	//VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
	//for( UINT i = 0; i < points_count; ++i ){
	//	s3f* v = &arr[i].verticles;
	//	v->x = vertex_origin.x + (v->x - vertex_origin.x) * scale->x;
	//	v->y = vertex_origin.y + (v->y - vertex_origin.y) * scale->y;
	//	v->z = vertex_origin.z + (v->z - vertex_origin.z) * scale->z;
	//}
}


void GLBrush::move( float dx, float dy, float dz )
{
	s3f delta( dx, dy, dz );
	vertex_origin += delta;
	for( UINT i = 0; i < points_count; ++i ){
		vertex_points[i] += delta;
	}

	update_points();
	//VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
	//for( UINT i = 0; i < points_count; ++i ){
	//	arr[i].verticles += delta;
	//}
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


void GLBrush::update_points( )
{
	VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
	glm::mat4x4 rot = glm::make_mat4x4(Camera::inversed_rotation());
	for( UINT i = 0; i < points_count; ++i ){
		if( !isFaced() ){
			arr[i].verticles = vertex_points[i];
		}else if( isScreen() ){
			glm::vec4 pt = glm::vec4(vertex_points[i].x, vertex_points[i].y, vertex_points[i].z, 1.0);
			pt = rot * pt;
			arr[i].verticles.x = pt.x;
			arr[i].verticles.y = pt.y;
			arr[i].verticles.z = pt.z;
		}else{
			glm::vec4 pt = glm::vec4(vertex_points[i].x - vertex_origin.x,
									 vertex_points[i].y - vertex_origin.y,
									 vertex_points[i].z - vertex_origin.z, 1.0);
			pt = rot * pt;
			arr[i].verticles.x = pt.x + vertex_origin.x;
			arr[i].verticles.y = pt.y + vertex_origin.y;
			arr[i].verticles.z = pt.z + vertex_origin.z;
		}
	}
}


VertexV2FT2FC4UI* GLBrush::points(  )
{
	return VBOArray::pointer( point_index );
}
