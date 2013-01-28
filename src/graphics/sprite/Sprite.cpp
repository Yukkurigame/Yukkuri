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
		tex->getSubTexture(pic, points, brush.points_count);
	}else{
		rect2f s(0.0, 0.0, 1.0, 1.0);
		init_coords( points, &s );
	}
}


void Sprite::resize( float w, float h )
{
	float dw, dh;
	dw = dh = 1.0;
	if( w > 0 ){
		//vertices.rb.x = vertices.rt.x = vertices.lb.x + w;
		if( !rect.width )
			rect.width = 1.0;
		dw = w / rect.width;
		rect.width = w;
	}
	if( h > 0 ){
		//vertices.rt.y = vertices.lt.y = vertices.lb.y + h;
		if( !rect.height )
			rect.height = 1.0;
		dh = h / rect.height;
		rect.height = h;
	}
	brush.scale( dw, dh );
}
