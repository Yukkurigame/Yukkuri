/*
 * Sprite.cpp
 *
 *  Created on: 26.05.2012
 *
 *
 *
 */

#include "graphics/sprite/Sprite.h"
#include "graphics/GraphicsTypes.h"
#include "graphics/render/Textures.h"
#include "graphics/utils/gl_shader.h"


void Sprite::setPicture( int pic )
{
	picture = pic;
	VertexV2FT2FC4UI* points = brush.points();
	if( !points || brush.points_count < 4 )
		return;
	TextureInfo* tex = Textures::get_pointer( texid );
	if( texid && tex ){
		tex->getSubTexture(pic, points, brush.points_count, brush.texture_indices );
	}else{
		rect2f s(0.0, 0.0, 1.0, 1.0);
		init_coords( points, &s, brush.texture_indices );
	}
}


void Sprite::resize( float x, float y, float z )
{
	s3f scale(1.0);
	if( x > 0 ){
		if( !size.x )
			size.x = 1.0;
		scale.x = x / size.x;
		size.x = x;
	}
	if( y > 0 ){
		if( !size.y )
			size.y = 1.0;
		scale.y = y / size.y;
		size.y = y;
	}
	if( z > 0 ){
		if( !size.z )
			size.z = 1.0;
		scale.z = z / size.z;
		size.z = z;
	}
	brush.scale( &scale );
}
