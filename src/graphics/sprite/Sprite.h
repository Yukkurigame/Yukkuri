/*
 * Sprite.h
 *
 *  Created on: 06.12.2012
 */

#ifndef SPRITE_H_
#define SPRITE_H_

#include "graphics/sprite/Brush.h"
#include "graphics/sprite/Material.h"
#include "graphics/GraphicsTypes.h"
#include "stddef.h"



struct Sprite
{
	unsigned int texid;
	unsigned int picture;
	list< GLuint > textures; // All textures uses single coordinates
	UINT material;
	unsigned int flags; // 1 - visible
						// 2 - centered
	rect2f rect;
	GLBrush brush;


	inline unsigned char isVisible()	{ return flags & 1; }
	inline void setVisible()			{ flags |= 1; }
	inline void clearVisible()			{ flags &= ~1; }

	inline unsigned char isCentered()	{ return brush.isCentered(); }

	inline void setFixed( ){ CHANGE_MATERIAL_FLAG( material, add_flag, glsFixed ) }
	inline void clearFixed( ){ CHANGE_MATERIAL_FLAG( material, clear_flag, glsFixed) }
	inline void setLight( ){ CHANGE_MATERIAL_FLAG( material, add_flag, glsLight ) }
	inline void clearLight( ){ CHANGE_MATERIAL_FLAG( material, clear_flag, glsLight ) }

	inline void addNormalMap( GLuint map ){
		textures.push_back( map );
		if( map ){
			CHANGE_MATERIAL_FLAG( material, add_flag, glsNormals )
		}
	}
	inline void removeNormalMap( ){
		// FIXME: will not work with textures after normal map
		textures.remove( textures.tail );
		CHANGE_MATERIAL_FLAG( material, clear_flag, glsNormals )
	}

	Sprite() : rect(), brush( prQUADS, 0 ) {
		material = GLMaterialManager::get( glsLight );
		picture = texid = 0;
		flags = 1; // visible only
	}

	Sprite( enum primitives shape, short centered ) : rect(), brush( shape, centered ){
		material = GLMaterialManager::get( glsLight );
		picture = texid = 0;
		flags = 1; // visible only
	}

	Sprite( Sprite* src ) : texid(src->texid), picture(src->picture),
		material(src->material), flags(src->flags), rect(src->rect),
		brush(src->brush) { }

	void setPicture( int pic );

	void resize( float w, float h );

	inline void move( float dx, float dy, float dz ){
		brush.move( dx, dy, dz );
	}

	inline void setPosition( const s3f* pos ){
		brush.set_position( pos->x, pos->y, pos->z );
	}

	inline void setPosition( float x, float y, float z ){
		brush.set_position( x, y, z );
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


inline bool compareSprites( Sprite* s1, Sprite* s2 )
{
	s3f* o1 = &s1->brush.vertex_origin;
	s3f* o2 = &s2->brush.vertex_origin;
	if( o1->z == o2->z ){
		if( o1->y == o2->y ){
			return ( o1->x > o2->x );
		}
		return ( o1->y > o2->y );
	}
	return ( o1->z < o2->z );
}


#endif /* SPRITE_H_ */
