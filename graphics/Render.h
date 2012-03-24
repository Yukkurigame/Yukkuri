/*
 * Render.h
 *
 *  Created on: 19.03.2012
 */

#ifndef RENDER_H_
#define RENDER_H_

#include "GraphicsTypes.h"

#include <string>
#include <vector>
#include <map>

struct ViewPoint {
	int x;
	int y;
	int z;
	ViewPoint(): x(), y(), z() {};
};


class RenderManager
{
public:
	static RenderManager* Instance( ){
		if( !graph )
			graph = new RenderManager();
		return graph;
	}

	static void Destroy( ){
		delete graph;
		graph = 0;
	}

	void openglInit();
	void openglSetup( int wwidth, int wheight );

	// Textures
	bool LoadTextures( );

	void AddTexture( std::string name );
	TextureInfo* GetTextureById( std::string id );

	GLuint* GetGLTexture( std::string name );
	GLuint* LoadGLTexture( std::string name );
	void AddGLTexture( std::string name, GLuint* tex );


	// Sprites
	inline Sprite* CreateGLSprite( float x, float y, float z, int width, int height ){
		return CreateGLSprite( x, y, z, width, height, -1, 0);
	}
	Sprite* CreateGLSprite( float x, float y, float z, int width, int height, int texture_id,
							int picture, short centered = 0 );
	Sprite* CreateGLSprite( float x, float y, float z, int width, int height, TextureInfo* tex,
							int picture, short centered = 0 );
	void FreeGLSprite( Sprite* sprite );


	bool CreateAtlas( );

	void MoveGlScene( int x, int y, int z );
	void DrawGLScene();
	void CleanGLScene();


private:
	RenderManager();
	~RenderManager();
	RenderManager( const RenderManager& );
	RenderManager& operator= ( const RenderManager& );
	static RenderManager* graph;

	int texturesCount;
	TextureInfo* textures;
	std::vector < TextureS* > internalTextures;
	std::map < std::string, GLuint* > texturesCache;

	void FreeGLTexture( GLuint* );
	void ClearGLTexturesCache( );

	std::vector < Sprite* > GLSprites;

	int verticlesSize;
	VertexV2FT2FC4UI* verticles;

	void ExtendVerticles( int count );

	GLuint VBOHandle;

	// Returns count of breakings
	int PrepareVBO( VBOStructureHandle* v );


	int minAtlasSize;
	int maxAtlasSize;
	int atlasWidth, atlasHeight;
	GLuint* atlasHandle;

	bool BuildAtlasMap( );
	bool BuildAtlas( );

	ViewPoint vpoint;
};


#endif /* RENDER_H_ */
