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
#include "graphics/utils/gl_shader.h"


void Sprite::setFixed()
{
	flags |= 4;
	shader = Shaders::getProgram( "fixed" );
}


void Sprite::setPicture( int pic )
{
	picture = pic;
	VertexV2FT2FC4UI* points = brush.points();
	if( !points || brush.points_count < 4 )
		return;
	if( texid && tex ){
		tex->getSubTexture(pic, points, brush.points_count);
	}else{
		rect2f s(0.0, 0.0, 1.0, 1.0);
		init_coords( points, &s );
		/*for( int i=0; i < qcLAST; ++i ){
			s2f& coords = points[i].coordinates;
			switch(i){
				case qcRT:
					coords = s2f(1.0, 1.0);
					break;
				case qcRB:
					coords = s2f(1.0, 0.0);
					break;
				case qcLT:
					coords = s2f(0.0, 1.0);
					break;
				case qcLB:
					coords = s2f(0.0, 1.0);
					break;
			}
		}*/
		/*
			coordinates.lb.x = coordinates.lt.x = 0;
			coordinates.lb.y = coordinates.rb.y = 0;
			coordinates.rb.x = coordinates.rt.x = 1.0;
			coordinates.lt.y = coordinates.rt.y = 1.0;
		 */
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
