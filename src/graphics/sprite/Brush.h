/*
 * Brush.h
 *
 *  Created on: 05.12.2012
 */

#ifndef GLBRUSH_H_
#define GLBRUSH_H_

#include "basic_types.h"
#include "graphics/GraphicsTypes.h"


struct GLBrush
{
	int mesh;
	GLuint method;
	s3f vertex_origin;
	UINT flags;
	UINT points_count;
	UINT point_index;
	UINT indices_count;
	UINT* indices_list;
	UINT texture_indices[4];
	UINT vertex_indices[4];

	GLBrush( );
	~GLBrush( );

	short init( int t );
	short init( const char* t );

	inline unsigned char isCentered()	{ return flags & 1; }
	void setCentered( );
	void clearCentered( );

	void resize_verticles( int size );
	void scale( const s3f* scale );
	void move( float dx, float dy, float dz );
	void set_color( const s4ub& color );
	s4ub get_color(  );

	inline void set_position( float x, float y, float z ){
		move( x - vertex_origin.x, y - vertex_origin.y, z - vertex_origin.z );
	}

	VertexV2FT2FC4UI* points(  );
};



#endif /* GLBRUSH_H_ */
