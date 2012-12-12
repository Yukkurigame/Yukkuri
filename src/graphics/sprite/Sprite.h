/*
 * Sprite.h
 *
 *  Created on: 06.12.2012
 */

#ifndef SPRITE_H_
#define SPRITE_H_

#include "graphics/sprite/Brush.h"
#include "graphics/GraphicsTypes.h"
#include "stddef.h"



struct Sprite
{
	unsigned int texid;
	unsigned int picture;
	GLuint atlas;		// Texture atlas, same as in tex probably
	unsigned int flags; // 1 - visible
						// 2 - centred
						// 4 - fixed
	GLuint shader;
	rect2f rect;
	TextureInfo* tex;
	GLBrush brush;
	//vertex3farr vertices;
	//coord2farr coordinates;
	//color4u clr;


	inline unsigned char isVisible()	{ return flags & 1; }
	inline void setVisible()			{ flags |= 1; }
	inline void clearVisible()			{ flags &= ~1; }

	inline unsigned char isCentered()	{ return brush.isCentered(); }

	inline unsigned char isFixed()		{ return flags & 4; }
	void setFixed();					// Needs shader placing
	inline void clearFixed()			{ flags &= ~4; shader = 0; }

	Sprite( short centered = 0 ) : rect(), brush( prQUADS, centered){
		tex = NULL;
		picture = atlas = texid = 0;
		flags = 1; // visible only
		shader = 0;
	}

	Sprite( Sprite* src ) : texid(src->texid), picture(src->picture),
		atlas(src->atlas), flags(src->flags), shader(src->shader),
		rect(src->rect), tex(src->tex), brush(src->brush) { }
		//, coordinates(src->coordinates), clr(src->clr)

	void setPicture( int pic );

	void resize( float w, float h );

	inline void move( float dx, float dy, float dz ){
		brush.move( dx, dy, dz );
	}

	inline void setPosition( float x, float y, float z ){
		brush.set_position( x, y, z );
		/*
		float width = vertices.rb.x - vertices.lb.x;
		float height = vertices.rt.y - vertices.lb.y; // FIXME: lb is rb?
		rect.x = x;
		rect.y = y;
		if( isCentered() ){
			float halfwidth = width/2;
			float halfheight = height/2;
			vertices.lb.x = vertices.lt.x = x - halfwidth;
			vertices.lb.y = vertices.rb.y = y - halfheight;
			vertices.rb.x = vertices.rt.x = x + halfwidth;
			vertices.lt.y = vertices.rt.y = y + halfheight;
		}else{
			vertices.lb.x = vertices.lt.x = x;
			vertices.lb.y = vertices.rb.y = y;
			vertices.rb.x = vertices.rt.x = x + width;
			vertices.lt.y = vertices.rt.y = y + height;
		}
		*/
	}

	inline void setPosition( float x, float y ){
		setPosition( x, y, brush.vertex_origin.z );
	}

	inline void toggleVisibility( ){
		if( isVisible() )
			clearVisible();
		else
			setVisible();
	}

};


#endif /* SPRITE_H_ */
