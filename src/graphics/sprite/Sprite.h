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
	GLuint atlas;		// Texture atlas, same as in tex probably
	GLuint normals;		// Texture normal map, same as in tex probably
	GLMaterial material;
	unsigned int flags; // 1 - visible
						// 2 - centred
						// 4 - fixed
	//GLuint shader;
	rect2f rect;
	TextureInfo* tex;
	GLBrush brush;


	inline unsigned char isVisible()	{ return flags & 1; }
	inline void setVisible()			{ flags |= 1; }
	inline void clearVisible()			{ flags &= ~1; }

	inline unsigned char isCentered()	{ return brush.isCentered(); }

	inline void setFixed( ){ material.add_flag(glsFixed); }
	inline void clearFixed( ){ material.clear_flag(glsFixed); }
	inline void setLight( ){ material.add_flag(glsLight); }
	inline void clearLight( ){ material.clear_flag(glsLight); }

	inline void addNormalMap( GLuint map ){
		normals = map;
		if( map )
			material.add_flag( glsNormals );
	}
	inline void removeNormalMap( ){
		normals = 0;
		material.clear_flag( glsNormals );
	}

	Sprite() : rect(), brush( prQUADS, 0 ) {
		tex = NULL;
		material.init_flags(glsLight);
		picture = atlas = normals = texid = 0;
		flags = 1; // visible only
	}

	Sprite( enum primitives shape, short centered ) : rect(), brush( shape, centered ){
		tex = NULL;
		material.init_flags(glsLight);
		picture = atlas = normals = texid = 0;
		flags = 1; // visible only
	}

	Sprite( Sprite* src ) : texid(src->texid), picture(src->picture),
		atlas(src->atlas), normals(src->normals),
		flags(src->flags), rect(src->rect), tex(src->tex),
		brush(src->brush) {
		material.init_flags( src->material.flags );
	}

	void setPicture( int pic );

	void resize( float w, float h );

	inline void move( float dx, float dy, float dz ){
		brush.move( dx, dy, dz );
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


#endif /* SPRITE_H_ */
