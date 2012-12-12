/*
 * Brush.h
 *
 *  Created on: 05.12.2012
 */

#ifndef GLBRUSH_H_
#define GLBRUSH_H_

#include "basic_types.h"
#include "graphics/utils/VBOArray.h"


struct GLBrush
{
	GLuint type;
	s3f vertex_origin;
	int point_index;
	//s3f* vertices;
	signed int points_count;
	GLBrush( GLuint t );
	~GLBrush( );

	void resize_verticles( int size ){
		VBOArray::freeSpace( point_index, points_count );
		points_count = size;
		point_index = VBOArray::getSpace( points_count );
		if( points_count < 1 )
			return;
		VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
		for( int i = 0; i < size; ++i ){
			arr->verticles = s3f();
			arr->coordinates = s2f();
			arr->color = s4ub();
			arr++;
		}
		/*vertices = (s3f*)realloc( vertices, sizeof(s3f)*size );
		if( size > vertices_count ){
			for( int i = vertices_count + 1; i < size; ++i )
				vertices[vertices_count] = vertex_origin;
		}
		vertices_count = size;
		return vertices;
		*/
	}

	void scale( float x, float y ){
		if( points_count < 2 )
			return;
		VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
		for( int i=0; i < points_count; ++i ){
			s3f* v = &arr[i].verticles;
			v->x += (v->x - vertex_origin.x) * x;
			v->y += (v->y - vertex_origin.y) * y;
		}
	}

	void set_position( float x, float y, float z ){
		s3f delta(  vertex_origin.x - x,
					vertex_origin.y - y,
					vertex_origin.z - z );
		vertex_origin -= delta;
		VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
		for( int i=0; i < points_count; ++i )
			arr[i].verticles -= delta;
	}

	void set_quad( s3f lb, s3f lt, s3f rt, s3f rb ){
		if( points_count != 4 )
			return;
		VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
		arr[0].verticles = lt + vertex_origin;
		arr[1].verticles = lb + vertex_origin;
		arr[2].verticles = rt + vertex_origin;
		arr[3].verticles = rb + vertex_origin;
	}

	inline void move( float dx, float dy, float dz ){
		vertex_origin.x += dx;
		vertex_origin.y += dy;
		vertex_origin.z += dz;
		VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index );
		for( int i=0; i < points_count; ++i ){
			s3f* v = &arr[i].verticles;
			v->x += dx;
			v->y += dy;
			v->z += dz;
		}
	}

	inline VertexV2FT2FC4UI* points(  ){
		return VBOArray::pointer( point_index );
	}

	// Set of quad functions
	// TODO: get rid of this
#define CORNER(name, index)			\
	s3f* name( ){					\
		if( points_count < index )	\
			return 0;				\
		VertexV2FT2FC4UI* arr = VBOArray::pointer( point_index ); \
		return &arr[index].verticles;	\
	}

	CORNER(lt, 0);
	CORNER(lb, 1);
	CORNER(rt, 2);
	CORNER(rb, 3);

#undef CORNER
};



#endif /* GLBRUSH_H_ */
