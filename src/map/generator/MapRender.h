/*
 * MapGen.h
 *
 *  Created on: 03.03.2013
 */

#ifndef MAPRENDER_H_
#define MAPRENDER_H_

#include "map/generator/generator_constants.h"
#include "map/generator/MapGenerator.h"
#include "map/generator/NoisyEdges.h"
#include "map/generator/Watersheds.h"
#include "map/generator/Histogram.h"
#include "graphics/GraphicsTypes.h"


class MapRender
{
public:
	MapRender( );
	~MapRender( );

	// Random parameters governing the overall shape of the island
	void newIsland( IslandForm type, const char* seed );

	void graphicsReset( ) {	}

	void go( );

	// Show some information about the maps
	void computeHistogram( float** hs, int* count, bucketFn fn );
	void drawHistogram( float x, float y, bucketFn fn, colorFn cfn,
			float width, float height, list< VertexV2FT2FC4UI* >* lines );
	void drawDistribution( float x, float y, bucketFn fn, colorFn cfn,
			float width, float height, list< VertexV2FT2FC4UI* >* lines );
	void drawHistograms( int picture );

	// Draw the map in the current map mode
	void drawMap( GeneratorMode, int picture );

	// 3D rendering of polygons. If the 'triangles3d' array is empty,
	// it's filled and the graphicsData is filled in as well. On
	// rendering, the triangles3d array has to be z-sorted and then
	// the resulting polygon data is transferred into graphicsData
	// before rendering.
	void render3dPolygons( );

	// Render the interior of polygons
	void renderPolygons( int picture );
	void renderGradientPolygions( int picture, UINT color_low, UINT color_high, NodeProperty,
			float min = 0.0, float max = 1.0 );

	// Render bridges across every narrow river edge. Bridges are
	// straight line segments perpendicular to the edge. Bridges are
	// drawn after rivers. TODO: sometimes the bridges aren't long
	// enough to cross the entire noisy line river. TODO: bridges
	// don't line up with curved road segments when there are
	// roads. It might be worth making a shader that draws the bridge
	// only when there's water underneath.
	void renderBridges( int picture );

	// Render roads. We draw these before polygon edges, so that rivers overwrite roads.
	void renderRoads( int picture );

	// Render the exterior of polygons: coastlines, lake shores,
	// rivers, lava fissures. We draw all of these after the polygons
	// so that polygons don't overwrite any edges.
	void renderEdges( int picture );

	// Render the polygons so that each can be seen clearly
	void renderDebugPolygons( int picture );

	// Render the paths from each polygon to the ocean, showing watersheds
	void renderWatersheds( int picture );

private:
	static const int SIZE = 600;

	// Island shape is controlled by the islandRandom seed and the
	// type of island. The islandShape function uses both of them to
	// determine whether any point should be water or land.
	IslandForm islandType;
	Texture atlas;
	UINT texture_id;
	//UINT atlas;

	// The map data

	MapGenerator* map;
	//Roads roads;
	//Lava lava
	Watersheds watersheds;
	NoisyEdges noisyEdges;

	void prepareLine( Edge* edge, float thikness, UINT color,
			list< VertexV2FT2FC4UI* >* lines, int alpha = 255 );
	void preparePolygon( Center* p, Edge* edge, UINT color,
				list< VertexV2FT2FC4UI* >* lines, int alpha = 255 );
	void draw( list< VertexV2FT2FC4UI* >& verticles, int picture );

	// Helper function for drawing triangles with gradients. This
	// function sets up the fill on the graphics object, and then
	// calls fillFunction to draw the desired path.
	//void drawGradientTriangle( s3f v1, s3f v2, s3f v3, colors[] colors:Array, fillFunction:Function)

};

#endif /* MAPRENDER_H_ */
