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

	int PushTexture( TextureProxy*, GLuint atlas );
	void PushTextures( std::vector < TextureProxy* >& tarray, GLuint atlas );

	int GetTextureNumberById( std::string id );
	TextureInfo* GetTextureByNumber( unsigned int number );

	bool DrawToGLTexture( GLuint ahandle, std::vector< TextureProxy* >* textures );

	// Sprites
	inline Sprite* CreateGLSprite( float x, float y, float z, int width, int height ){
		return CreateGLSprite( x, y, z, width, height, 0, 0);
	}
	Sprite* CreateGLSprite( float x, float y, float z, int width, int height, int texture_id,
							int picture, short centered = 0 );
	Sprite* CreateGLSprite( Sprite* osprite );
	void FreeGLSprite( Sprite* sprite );
	void FreeGLSprites( std::vector< Sprite* >* sprites );

	bool CreateAtlas( GLuint * atlas, int* width, int* height );


	inline void MoveGlScene( int x, int y, int z ){
		vpoint.x = x;
		vpoint.y = y;
		vpoint.z = z;
		MoveGlScene();
	}
	void MoveGlScene( );
	void DrawGLScene( );
	void CleanGLScene( );


	Texture* LoadGLTexture( std::string name );

private:
	RenderManager();
	~RenderManager();
	RenderManager( const RenderManager& );
	RenderManager& operator= ( const RenderManager& );
	static RenderManager* graph;

	int texturesCount;
	TextureInfo* textures;



	std::map < std::string, Texture* > texturesCache;
	Texture* GetGLTexture( std::string name );
	void AddGLTexture( std::string name, Texture* tex );
	void FreeGLTexture( Texture* );
	void ClearGLTexturesCache( );


	std::vector < Sprite* > GLSprites;

	int verticlesSize;
	VertexV2FT2FC4UI* verticles;

	void ExtendVerticles( );

	GLuint VBOHandle;

	// Returns count of breakings
	VBOStructureHandle* PrepareVBO( int* count );
	//int VBOHandlesCount;
	//int VBOHandlesSize;
	//VBOStructureHandle* VBOHandles;

	int atlasWidth, atlasHeight;
	GLuint atlasHandle;

	bool BuildAtlasMap( int* width, int* height );
	bool BuildAtlas( GLuint* ahandle, int width, int height );

	s3f vpoint;


	void TestDrawAtlas(int x, int y, GLuint atlas);
};


#endif /* RENDER_H_ */
