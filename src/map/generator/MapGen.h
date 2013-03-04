/*
 * MapGen.h
 *
 *  Created on: 03.03.2013
 */

#ifndef MAPGEN_H_
#define MAPGEN_H_

#include "map/generator/generator_constants.h"
#include "map/generator/MapGenerator.h"
#include "graphics/sprite/Sprite.h"
#include <string>


class MapGen
{
private:
	static const int SIZE = 600;

	// Island shape is controlled by the islandRandom seed and the
	// type of island. The islandShape function uses both of them to
	// determine whether any point should be water or land.
	IslandForm islandType;
	std::string islandSeedInitial;
	Sprite* sprite;

	// The map data

	MapGenerator* map;
	//Roads roads;
	//Lava lava
	//Watersheds watersheds;
	//NoisyEdges noisyEdges;

public:
	MapGen( );
	~MapGen( );

	// Random parameters governing the overall shape of the island
	void newIsland( IslandForm type );

	void graphicsReset( )
	{
	}

	void go( IslandForm type );

	// Show some information about the maps
	//private static var _biomeMap:Array =
	//  ['BEACH', 'LAKE', 'ICE', 'MARSH', 'SNOW', 'TUNDRA', 'BARE', 'SCORCHED',
	//   'TAIGA', 'SHRUBLAND', 'TEMPERATE_DESERT', 'TEMPERATE_RAIN_FOREST',
	//   'TEMPERATE_DECIDUOUS_FOREST', 'GRASSLAND', 'SUBTROPICAL_DESERT',
	//   'TROPICAL_RAIN_FOREST', 'TROPICAL_SEASONAL_FOREST'];

	void drawHistograms( );

	// Helper functions for rendering paths
	void drawPathForwards( std::vector<s2f>& path );
	void drawPathBackwards( std::vector<s2f>& path );

	// Helper function for color manipulation. When f==0: color0, f==1: color1
	unsigned int interpolateColor( unsigned int color0, unsigned int color1, double f );

	// Helper function for drawing triangles with gradients. This
	// function sets up the fill on the graphics object, and then
	// calls fillFunction to draw the desired path.
//private:
	//void drawGradientTriangle( s3f v1, s3f v2, s3f v3, colors[] colors:Array, fillFunction:Function)
//public:

	// Draw the map in the current map mode
	void drawMap( GeneratorMode );

	// 3D rendering of polygons. If the 'triangles3d' array is empty,
	// it's filled and the graphicsData is filled in as well. On
	// rendering, the triangles3d array has to be z-sorted and then
	// the resulting polygon data is transferred into graphicsData
	// before rendering.
	void render3dPolygons( );

	// Render the interior of polygons
	void renderPolygons( );

	// Render bridges across every narrow river edge. Bridges are
	// straight line segments perpendicular to the edge. Bridges are
	// drawn after rivers. TODO: sometimes the bridges aren't long
	// enough to cross the entire noisy line river. TODO: bridges
	// don't line up with curved road segments when there are
	// roads. It might be worth making a shader that draws the bridge
	// only when there's water underneath.
	void renderBridges( );

	// Render roads. We draw these before polygon edges, so that rivers overwrite roads.
	void renderRoads( );

	// Render the exterior of polygons: coastlines, lake shores,
	// rivers, lava fissures. We draw all of these after the polygons
	// so that polygons don't overwrite any edges.
	void renderEdges( );

	// Render the polygons so that each can be seen clearly
	void renderDebugPolygons( );

	// Render the paths from each polygon to the ocean, showing watersheds
	void renderWatersheds( );

	// 3D Render must make this shit, not program
	/*double function calculateLighting(p:Center, r:Corner, s:Corner) {
	 var A:Vector3D = new Vector3D(p.point.x, p.point.y, p.elevation);
	 var B:Vector3D = new Vector3D(r.point.x, r.point.y, r.elevation);
	 var C:Vector3D = new Vector3D(s.point.x, s.point.y, s.elevation);
	 var normal:Vector3D = B.subtract(A).crossProduct(C.subtract(A));
	 if (normal.z < 0) { normal.scaleBy(-1); }
	 normal.normalize();
	 var light:Number = 0.5 + 35*normal.dotProduct(lightVector);
	 if (light < 0) light = 0;
	 if (light > 1) light = 1;
	 return light;
	 }
	 public function colorWithSlope(color:int, p:Center, q:Center, edge:Edge):int {
	 var r:Corner = edge.v0;
	 var s:Corner = edge.v1;
	 if (!r || !s) {
	 // Edge of the map
	 return displayColors.OCEAN;
	 } else if (p.water) {
	 return color;
	 }

	 if (q != null && p.water == q.water) color = interpolateColor(color, displayColors[q.biome], 0.4);
	 var colorLow:int = interpolateColor(color, 0x333333, 0.7);
	 var colorHigh:int = interpolateColor(color, 0xffffff, 0.3);
	 var light:Number = calculateLighting(p, r, s);
	 if (light < 0.5) return interpolateColor(colorLow, color, light*2);
	 else return interpolateColor(color, colorHigh, light*2-1);
	 }
	 public function colorWithSmoothColors(color:int, p:Center, q:Center, edge:Edge):int {
	 if (q != null && p.water == q.water) {
	 color = interpolateColor(displayColors[p.biome], displayColors[q.biome], 0.25);
	 }
	 return color;
	 }
	 */

};

#endif /* MAPGEN_H_ */
