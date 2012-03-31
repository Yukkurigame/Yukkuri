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


	std::map < std::string, Texture* > texturesCache;
	Texture* GetGLTexture( std::string name );
	Texture* LoadGLTexture( std::string name );
	void AddGLTexture( std::string name, Texture* tex );
	void FreeGLTexture( Texture* );
	void ClearGLTexturesCache( );


	std::vector < Sprite* > GLSprites;

	int verticlesSize;
	VertexV2FT2FC4UI* verticles;

	void ExtendVerticles( );

	//GLuint VBOHandle;

	// Returns count of breakings
	VBOStructureHandle* PrepareVBO( int* count );


	int minAtlasSize;
	int maxAtlasSize;
	int atlasWidth, atlasHeight;
	GLuint* atlasHandle;

	bool BuildAtlasMap( );
	GLuint BuildAtlas( );

	s3f vpoint;


	void TestDrawAtlas(int x, int y);
};


#endif /* RENDER_H_ */
