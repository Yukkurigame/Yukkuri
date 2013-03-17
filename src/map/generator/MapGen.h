/*
 * MapGen.h
 *
 *  Created on: 03.03.2013
 */

#ifndef MAPGEN_H_
#define MAPGEN_H_

#include "map/generator/generator_constants.h"
#include "map/generator/MapGenerator.h"
#include "map/generator/NoisyEdges.h"
#include "map/generator/Watersheds.h"
#include "graphics/GraphicsTypes.h"
#include <string>


class MapGen
{
public:
	MapGen( );
	~MapGen( );

	// Random parameters governing the overall shape of the island
	void newIsland( IslandForm type, const char* seed );

	void graphicsReset( ) {	}

	void go( );

	// Show some information about the maps
	void drawHistograms( );

	// Helper function for color manipulation. When f==0: color0, f==1: color1
	unsigned int interpolateColor( unsigned int color0, unsigned int color1, double f );

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
	std::string islandSeedInitial;
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
			list< VertexV2FT2FC4UI* >& lines, int alpha = 255 );
	void draw( list< VertexV2FT2FC4UI* >& verticles, int picture );

	// Helper function for drawing triangles with gradients. This
	// function sets up the fill on the graphics object, and then
	// calls fillFunction to draw the desired path.
	//void drawGradientTriangle( s3f v1, s3f v2, s3f v3, colors[] colors:Array, fillFunction:Function)

};

#endif /* MAPGEN_H_ */
