/*
 * Brush.cpp
 *
 *  Created on: 05.12.2012
 */

#include "graphics/sprite/Brush.h"
#include "graphics/sprite/Mesh.h"
#include "graphics/render/VBuffer.h"
#include "graphics/Camera.h"


GLBrush::GLBrush( UINT VBO ) :
	mesh( -1 ), method(GL_TRIANGLES), VBOHandler(VBO)
{
	flags = 0;
	point_index = 0;
	points_count = 0;
	indices_count = 0;
	indices_list = NULL;
	vertex_points = NULL;
}


GLBrush::~GLBrush( )
{
	VBufferHandler* hdl = VBuffer::handler( VBOHandler );
	if( hdl )
		hdl->array.freeSpace( point_index, points_count );
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

	VBufferHandler* hdl = VBuffer::handler( VBOHandler );
	if( !hdl )
		return;

	s3f dist = size( vertex_points, points_count );
	VertexV2FT2FC4UI* arr = hdl->array.pointer( point_index );
	for( UINT i = 0; i < points_count; ++i ){
		s3f* v = isFaced() ? &vertex_points[i] : &arr[i].verticles;
		v->x -= dist.x / 2.0f;
		v->y -= dist.y / 2.0f;
		v->z -= dist.z / 2.0f;
	}

	setUpdated();
	flags |= glbCentered;
}

void GLBrush::clearCentered( )
{
	if( !isCentered() )
		return;
	VBufferHandler* hdl = VBuffer::handler( VBOHandler );
	if( !hdl )
		return;

	s3f dist = size( vertex_points, points_count );
	VertexV2FT2FC4UI* arr = hdl->array.pointer( point_index );
	for( UINT i = 0; i < points_count; ++i ){
		s3f* v = isFaced() ? &vertex_points[i] : &arr[i].verticles;
		v->x += dist.x / 2.0f;
		v->y += dist.y / 2.0f;
		v->z += dist.z / 2.0f;
	}

	setUpdated();
	flags &= ~glbCentered;
}

void GLBrush::resize_verticles( int size )
{
	VBufferHandler* hdl = VBuffer::handler( VBOHandler );
	if( !hdl )
		return;

	hdl->array.freeSpace( point_index, points_count );
	if( vertex_points )
		delete[] vertex_points, vertex_points = NULL;

	points_count = size;
	if( points_count < 1 )
		return;
	vertex_points = new s3f[points_count];

	point_index = hdl->array.getSpace( points_count );
	VertexV2FT2FC4UI* arr = hdl->array.pointer( point_index );
	for( int i = 0; i < size; ++i ){
		arr->verticles = s3f();
		arr->coordinates = s2f();
		arr->color = s4ub();
		arr++;
	}

	setUpdated();
}


void GLBrush::scale( const s3f* scale )
{
	if( points_count < 2 )
		return;

	VBufferHandler* hdl = VBuffer::handler( VBOHandler );
	if( !hdl )
		return;

	VertexV2FT2FC4UI* arr = hdl->array.pointer( point_index );
	for( UINT i = 0; i < points_count; ++i ){
		s3f* v = isFaced() ? &vertex_points[i] : &arr[i].verticles;
		v->x = vertex_origin.x + (v->x - vertex_origin.x) * scale->x;
		v->y = vertex_origin.y + (v->y - vertex_origin.y) * scale->y;
		v->z = vertex_origin.z + (v->z - vertex_origin.z) * scale->z;
	}

	setUpdated();
}


void GLBrush::move( float dx, float dy, float dz )
{
	VBufferHandler* hdl = VBuffer::handler( VBOHandler );
	if( !hdl )
		return;

	s3f delta( dx, dy, dz );
	vertex_origin += delta;
	VertexV2FT2FC4UI* arr = hdl->array.pointer( point_index );
	for( UINT i = 0; i < points_count; ++i ){
		s3f* v = isFaced() ? &vertex_points[i] : &arr[i].verticles;
		v->x += delta.x;
		v->y += delta.y;
		v->z += delta.z;
	}

	setUpdated();
}


void GLBrush::set_color( const s4ub& color )
{
	VBufferHandler* hdl = VBuffer::handler( VBOHandler );
	if( !hdl )
		return;

	VertexV2FT2FC4UI* arr = hdl->array.pointer( point_index );
	for( UINT i=0; i < points_count; ++i )
		arr[i].color.set(color);

	setUpdated();
}


s4ub GLBrush::get_color(  )
{
	VBufferHandler* hdl = VBuffer::handler( VBOHandler );
	if( !hdl )
		return s4ub();

	VertexV2FT2FC4UI* arr = hdl->array.pointer( point_index );
	return arr[0].color;
}


void GLBrush::update_points( )
{
	if( !isFaced() )
		return;

	VBufferHandler* hdl = VBuffer::handler( VBOHandler );
	if( !hdl )
		return;


	VertexV2FT2FC4UI* arr = hdl->array.pointer( point_index );
	float rot[4][4];
	memcpy( &rot[0][0], Camera::inversed_rotation(), (UINT)16 * sizeof(float) );
	for( UINT i = 0; i < points_count; ++i ){
		s3f& target = arr[i].verticles;
		if( isScreen() ){
			//glm::vec4 pt = glm::vec4(vertex_points[i].x, vertex_points[i].y, vertex_points[i].z, 1.0);
			//pt = rot * pt;
			s3f& v = vertex_points[i];
			target.x = rot[0][0] * v.x + rot[1][0] * v.y + rot[2][0] * v.z + rot[3][0] * 1.0;
			target.y = rot[0][1] * v.x + rot[1][1] * v.y + rot[2][1] * v.z + rot[3][1] * 1.0;
			target.z = rot[0][2] * v.x + rot[1][2] * v.y + rot[2][2] * v.z + rot[3][2] * 1.0;
		}else{
			s3f v = vertex_points[i] - vertex_origin;
			//glm::vec4 pt = glm::vec4(vertex_points[i].x - vertex_origin.x,
			//						vertex_points[i].y - vertex_origin.y,
			//						vertex_points[i].z - vertex_origin.z, 1.0);
			//pt = rot * pt;
			target.x = rot[0][0] * v.x + rot[1][0] * v.y + rot[2][0] * v.z + rot[3][0] * 1.0 + vertex_origin.x;
			target.y = rot[0][1] * v.x + rot[1][1] * v.y + rot[2][1] * v.z + rot[3][1] * 1.0 + vertex_origin.y;
			target.z = rot[0][2] * v.x + rot[1][2] * v.y + rot[2][2] * v.z + rot[3][2] * 1.0 + vertex_origin.z;
		}
	}

	clearUpdated();
}


VertexV2FT2FC4UI* GLBrush::points(  )
{
	VBufferHandler* hdl = VBuffer::handler( VBOHandler );
	if( !hdl )
		return NULL;
	return hdl->array.pointer( point_index );
}
