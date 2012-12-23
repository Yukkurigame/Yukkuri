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
	enum primitives type;
	s3f vertex_origin;
	int point_index;
	//s3f* vertices;
	signed int points_count;
	int flags;
	GLBrush( enum primitives t, short centered );
	~GLBrush( );

	inline unsigned char isCentered()	{ return flags & 1; }

	void resize_verticles( int size );
	void scale( float x, float y );
	void set_quad( s3f lb, s3f lt, s3f rt, s3f rb );
	void move( float dx, float dy, float dz );
	void set_color( const s4ub& color );
	s4ub get_color(  );

	inline void set_position( float x, float y, float z ){
		move( x - vertex_origin.x, y - vertex_origin.y, z - vertex_origin.z );
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

	CORNER(lt, qcLT);
	CORNER(lb, qcLB);
	CORNER(rt, qcRT);
	CORNER(rb, qcRB);

#undef CORNER
};



#endif /* GLBRUSH_H_ */