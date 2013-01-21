/*
 * Render.h
 *
 *  Created on: 19.03.2012
 */

#ifndef RENDER_H_
#define RENDER_H_

#include "graphics/sprite/Sprite.h"

#include <string>
#include <vector>


enum RenderMethod {
	rmSingle = 0,
	rmGBuffer = 1
};



namespace RenderManager
{
	void init();
	void clean( );

	void openglInit();
	bool openglSetup( int wwidth, int wheight );


	//////////////////////////////////////////////////
	// Textures

	bool LoadTextures( );

	int PushTexture( TextureProxy*, GLuint atlas, GLuint normals );
	void PushTextures( std::vector < TextureProxy* >& tarray, GLuint atlas, GLuint normals );

	int GetTextureNumberById( std::string id );
	TextureInfo* GetTextureByNumber( unsigned int number );

	bool DrawToGLTexture( GLuint ahandle, std::vector< TextureProxy* >* textures );


	//////////////////////////////////////////////////
	// Sprites
	Sprite* CreateGLSprite( float x, float y, float z, int width, int height, unsigned int texture_id,
							enum primitives shape, short centered = 0 );
	inline Sprite* CreateGLSprite( float x, float y, float z, int width, int height,
							unsigned int texture_id, int picture, short centered = 0 ){
		Sprite* s = CreateGLSprite( x, y, z, width, height, texture_id, prQUADS, centered );
		s->setPicture( picture );
		return s;
	}
	inline Sprite* CreateGLSprite( float x, float y, float z, int width, int height ){
			return CreateGLSprite( x, y, z, width, height, 0, 0 );
	}
	Sprite* CreateGLSprite( Sprite* osprite );
	void FreeGLSprite( Sprite* sprite );
	void FreeGLSprites( std::vector< Sprite* >* sprites );

	list< Sprite* >* GetSpritesArray( );


	//////////////////////////////////////////////////
	// Scene

	//void MoveGlScene( );
	void DrawGLScene( );
	void CleanGLScene( );

};


#endif /* RENDER_H_ */
