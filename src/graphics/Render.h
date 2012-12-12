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


namespace RenderManager
{
	void init();
	void clean( );

	void openglInit();
	void openglSetup( int wwidth, int wheight );


	//////////////////////////////////////////////////
	// Textures

	bool LoadTextures( );

	int PushTexture( TextureProxy*, GLuint atlas );
	void PushTextures( std::vector < TextureProxy* >& tarray, GLuint atlas );

	int GetTextureNumberById( std::string id );
	TextureInfo* GetTextureByNumber( unsigned int number );

	bool DrawToGLTexture( GLuint ahandle, std::vector< TextureProxy* >* textures );


	//////////////////////////////////////////////////
	// Sprites

	Sprite* CreateGLSprite( float x, float y, float z, int width, int height, unsigned int texture_id,
							int picture, short centered = 0 );
	inline Sprite* CreateGLSprite( float x, float y, float z, int width, int height ){
			return CreateGLSprite( x, y, z, width, height, 0, 0 );
	}
	Sprite* CreateGLSprite( Sprite* osprite );
	void FreeGLSprite( Sprite* sprite );
	void FreeGLSprites( std::vector< Sprite* >* sprites );


	//////////////////////////////////////////////////
	// Scene

	void MoveGlScene( );
	void DrawGLScene( );
	void CleanGLScene( );

	static s3f vpoint;

	inline void MoveGlScene( int x, int y, int z ){
		vpoint.x = (float)x;
		vpoint.y = (float)y;
		vpoint.z = (float)z;
		MoveGlScene();
	}

};


#endif /* RENDER_H_ */
