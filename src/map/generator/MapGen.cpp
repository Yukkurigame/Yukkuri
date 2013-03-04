/*
 * MapGen.cpp
 *
 *  Created on: 03.03.2013
 */

#include "map/generator/MapGen.h"
#include "graphics/Render.h"
#include "graphics/sprite/Mesh.h"
#include "hacks.h"
#include "debug.h"



MapGen::MapGen( )
{
	islandType = ifSquare;
	islandSeedInitial = "85882-1";
	map = new MapGenerator( SIZE );
	go( islandType );
}

MapGen::~MapGen( )
{
	// TODO Auto-generated destructor stub
}

void MapGen::newIsland( IslandForm type )
{
	int seed = 0;
	int variant = 0;

	// String seed
	if( seed == 0 ){
		std::string seed_str = islandSeedInitial;
		// Convert the string into a number. This is a cheesy way to
		// do it but it doesn't matter. It just allows people to use
		// words as seeds.
		for( UINT i = 0; i < seed_str.length(); ++i ){
			seed = ( seed << 4 ) | (UINT)seed_str[i];
		}
		seed %= 100000;
		variant = 1 + floor( 9 * random() );
	}
	islandType = type;
	map->newIsland( type, seed, variant );
}

void MapGen::go( IslandForm type )
{
	//roads = new Roads();
	//lava = new Lava();
	//watersheds = new Watersheds();
	//noisyEdges = new NoisyEdges();

	Debug::debug( Debug::MAP, "Shaping map...\n" );
	newIsland( type );

	map->go( 0, 1 );
	//drawMap();

	map->go( 1, 2 );
	//drawMap();

	map->go( 2, 3 );
	map->assignBiomes();
	//drawMap();

	Debug::debug( Debug::MAP, "Features...\n" );
	map->go( 3, 6 );
	map->assignBiomes();
	//drawMap();

	//roads.createRoads(map);
	// lava.createLava(map, map.mapRandom.nextDouble);
	//watersheds.createWatersheds(map);
	//noisyEdges.buildNoisyEdges(map, lava, map.mapRandom);
	drawMap( gm3d );
}

void MapGen::drawHistograms( )
{
/*	// There are pairs of functions for each chart. The bucket
	// function maps the polygon Center to a small int, and the
	// color function maps the int to a color.
	function
	landTypeBucket(p:Center):int{
		if (p.ocean) return 1;
		else if (p.coast) return 2;
		else if (p.water) return 3;
		else return 4;
	}
	function
	landTypeColor(bucket:int):uint{
		if (bucket == 1) return displayColors.OCEAN;
		else if (bucket == 2) return displayColors.BEACH;
		else if (bucket == 3) return displayColors.LAKE;
		else return displayColors.TEMPERATE_DECIDUOUS_FOREST;
	}
	function
	elevationBucket(p:Center):int{
		if (p.ocean) return -1;
		else return Math.floor(p.elevation*10);
	}
	function
	elevationColor(bucket:int):uint{
		return interpolateColor(displayColors.TEMPERATE_DECIDUOUS_FOREST,
				displayColors.GRASSLAND, bucket*0.1);
	}
	function
	moistureBucket(p:Center):int{
		if (p.water) return -1;
		else return Math.floor(p.moisture*10);
	}
	function
	moistureColor(bucket:int):uint{
		return interpolateColor(displayColors.BEACH, displayColors.RIVER, bucket*0.1);
	}
	function
	biomeBucket(p:Center):int{
		return _biomeMap.indexOf(p.biome);
	}
	function
	biomeColor(bucket:int):uint{
		return displayColors[_biomeMap[bucket]];
	}

	function
	computeHistogram(bucketFn:Function):Array{
		var p:Center, histogram:Array, bucket:int;
		histogram = [];
		for each (p in map.centers){
			bucket = bucketFn(p);
			if (bucket >= 0) histogram[bucket] = (histogram[bucket] || 0) + 1;
		}
		return histogram;
	}

	function
	drawHistogram(x:Number, y:Number, bucketFn:Function, colorFn:Function,
			width:Number, height:Number):void{
		var scale:Number, i:int;
		var histogram:Array = computeHistogram(bucketFn);

		scale = 0.0;
		for (i = 0; i < histogram.length; i++){
			scale = Math.max(scale, histogram[i] || 0);
		}
		for (i = 0; i < histogram.length; i++){
			if (histogram[i]){
				graphics.beginFill(colorFn(i));
				graphics.drawRect(SIZE+x+i*width/histogram.length, y+height,
						Math.max(0, width/histogram.length-1), -height*histogram[i]/scale);
				graphics.endFill();
			}
		}
	}

	function
	drawDistribution(x:Number, y:Number, bucketFn:Function, colorFn:Function,
			width:Number, height:Number):void{
		var scale:Number, i:int, x:Number, w:Number;
		var histogram:Array = computeHistogram(bucketFn);

		scale = 0.0;
		for (i = 0; i < histogram.length; i++){
			scale += histogram[i] || 0.0;
		}
		for (i = 0; i < histogram.length; i++){
			if (histogram[i]){
				graphics.beginFill(colorFn(i));
				w = histogram[i]/scale*width;
				graphics.drawRect(SIZE+x, y, Math.max(0, w-1), height);
				x += w;
				graphics.endFill();
			}
		}
	}

	var
	x:Number = 23, y
	:Number = 140, width:Number = 154;
	drawDistribution( x, y, landTypeBucket, landTypeColor, width, 20 );
	drawDistribution( x, y + 25, biomeBucket, biomeColor, width, 20 );

	drawHistogram( x, y + 55, elevationBucket, elevationColor, width, 30 );
	drawHistogram( x, y + 95, moistureBucket, moistureColor, width, 20 );
*/
}


void MapGen::drawPathForwards( std::vector<s2f>& path )
{
	for( UINT i = 0; i < path.size(); ++i ){
		//graphics.lineTo(path[i].x, path[i].y);
	}
}

void MapGen::drawPathBackwards( std::vector<s2f>& path )
{
	for( int i = path.size() - 1; i >= 0; --i ){
		//graphics.lineTo(path[i].x, path[i].y);
	}
}

unsigned int MapGen::interpolateColor( unsigned int c0, unsigned int c1, double f )
{
	unsigned int r = ( 1 - f ) * ( c0 >> 16 ) + f * ( c1 >> 16 );
	unsigned int g = ( 1 - f ) * ( ( c0 >> 8 ) & 0xff ) + f * ( ( c1 >> 8 ) & 0xff );
	unsigned int b = ( 1 - f ) * ( c0 & 0xff ) + f * ( c1 & 0xff );
	if( r > 255 )
		r = 255;
	if( g > 255 )
		g = 255;
	if( b > 255 )
		b = 255;
	return ( r << 16 ) | ( g << 8 ) | b;
}

/*
void drawGradientTriangle( s3f v1, s3f v2, s3f v3, colors[] colors:Array, fillFunction:Function)
var m:Matrix = new Matrix();

 // Center of triangle:
 var V:Vector3D = v1.add(v2).add(v3);
 V.scaleBy(1/3.0);

 // Normal of the plane containing the triangle:
 var N:Vector3D = v2.subtract(v1).crossProduct(v3.subtract(v1));
 N.normalize();

 // Gradient vector in x-y plane pointing in the direction of increasing z
 var G:Vector3D = new Vector3D(-N.x/N.z, -N.y/N.z, 0);

 // Center of the color gradient
 var C:Vector3D = new Vector3D(V.x - G.x*((V.z-0.5)/G.length/G.length), V.y - G.y*((V.z-0.5)/G.length/G.length));

 if (G.length < 1e-6) {
   // If the gradient vector is small, there's not much
   // difference in colors across this triangle. Use a plain
   // fill, because the numeric accuracy of 1/G.length is not to
   // be trusted.  NOTE: only works for 1, 2, 3 colors in the array
   var color:uint = colors[0];
   if (colors.length == 2) {
     color = interpolateColor(colors[0], colors[1], V.z);
   } else if (colors.length == 3) {
     if (V.z < 0.5) {
       color = interpolateColor(colors[0], colors[1], V.z*2);
     } else {
       color = interpolateColor(colors[1], colors[2], V.z*2-1);
     }
   }
   graphics.beginFill(color);
 } else {
   // The gradient box is weird to set up, so we let Flash set up
   // a basic matrix and then we alter it:
   m.createGradientBox(1, 1, 0, 0, 0);
   m.translate(-0.5, -0.5);
   m.scale((1/G.length), (1/G.length));
   m.rotate(Math.atan2(G.y, G.x));
   m.translate(C.x, C.y);
   var alphas:Array = colors.map(function (_:*, index:int, A:Array):Number { return 1.0; });
   var spread:Array = colors.map(function (_:*, index:int, A:Array):int { return 255*index/(A.length-1); });
   graphics.beginGradientFill(GradientType.LINEAR, colors, alphas, spread, m, SpreadMethod.PAD);
 }
 fillFunction();
 graphics.endFill();
}*/

void MapGen::drawMap( GeneratorMode mode )
{
	graphicsReset();
	//noiseLayer.visible = true;
	drawHistograms();

	switch( mode ){
		case gm3d:
			//noiseLayer.visible = false;
			render3dPolygons();
			return;
			break;
		case gmPolygons:
			//noiseLayer.visible = false;
			renderDebugPolygons();
			break;
		case gmWatersheds:
			//noiseLayer.visible = false;
			renderDebugPolygons();
			renderWatersheds();
			return;
			break;
		case gmBiome:
			renderPolygons();
			break;
		case gmSlopes:
			renderPolygons();
			break;
		case gmSmooth:
			renderPolygons();
			break;
		case gmElevation:
			renderPolygons();
			break;
		case gmMoisture:
			renderPolygons();
			break;
	}

	if( mode != gmSlopes && mode != gmMoisture )
		renderRoads();

	if( mode != gmPolygons )
		renderEdges();

	if( mode != gmSlopes && mode != gmMoisture )
		renderBridges();
}


void MapGen::render3dPolygons( )
{
    double zScale = 0.15*SIZE;

    s4ub colors[bLAST];

    if( sprite )
    	RenderManager::FreeGLSprite(sprite);

	int count = 0;
	// Calculate count of vertices
	FOREACHIT( map->centers ){
		FOREACH( tit, (*it)->borders ){
			count++;
		}
	}

	if( !count )
		return;

    int mesh = MeshManager::get("mesh_terrain");
    sprite = RenderManager::CreateGLSprite( 0, 0, 0, SIZE, SIZE, mesh );

	GLBrush& brush = sprite->brush;
	brush.init(-1);

	// Init vertex brush
	int vx_count = count * 3;
	brush.resize_verticles( vx_count );
	brush.indices_count = 0;
	brush.indices_list = (UINT*)malloc( (UINT)sizeof(UINT) * vx_count );
	VertexV2FT2FC4UI* arr = brush.points();

	FOREACHIT( map->centers ){
		Center* p = (*it);
		FOREACH( tit, p->borders ){
			Edge* edge = (*tit);
			const s4ub& color = colors[p->biome];
			Corner* corner0 = edge->v0;
			Corner* corner1 = edge->v1;

			if( !corner0 || !corner1 ){
			   // Edge of the map; we can't deal with it right now
			   continue;
			}

            float zp = zScale * p->elevation;
            float z0 = zScale * corner0->elevation;
            float z1 = zScale*corner1->elevation;
            // Verticles
            s3f vc[3] = {
            		s3f(p->point.x, p->point.y, zp),
            		s3f(corner0->point.x, corner0->point.y, z0),
            		s3f(corner1->point.x, corner1->point.y, z1)
            };

            // Add position & color to brush
            for( int i = 0; i < 3; ++i ){
            	int ti = brush.indices_count;
            	arr[ti].verticles = vc[i];
            	brush.indices_list[brush.indices_count++] = ti + brush.point_index;
            	arr[ti].color.set( color );
            }

		}
	}

	// Render engine cares about all other stuff, not me
}


void MapGen::renderPolygons( )
{
	/*
     var p:Center, r:Center;

     // My Voronoi polygon rendering doesn't handle the boundary
     // polygons, so I just fill everything with ocean first.
     graphics.beginFill(colors.OCEAN);
     graphics.drawRect(0, 0, SIZE, SIZE);
     graphics.endFill();

     for each (p in map.centers) {
         for each (r in p.neighbors) {
             var edge:Edge = map.lookupEdgeFromCenter(p, r);
             var color:int = colors[p.biome] || 0;
             if (colorOverrideFunction != null) {
               color = colorOverrideFunction(color, p, r, edge);
             }

             function drawPath0():void {
               var path:Vector.<Point> = noisyEdges.path0[edge.index];
               graphics.moveTo(p.point.x, p.point.y);
               graphics.lineTo(path[0].x, path[0].y);
               drawPathForwards(graphics, path);
               graphics.lineTo(p.point.x, p.point.y);
             }

             function drawPath1():void {
               var path:Vector.<Point> = noisyEdges.path1[edge.index];
               graphics.moveTo(p.point.x, p.point.y);
               graphics.lineTo(path[0].x, path[0].y);
               drawPathForwards(graphics, path);
               graphics.lineTo(p.point.x, p.point.y);
             }

             if (noisyEdges.path0[edge.index] == null
                 || noisyEdges.path1[edge.index] == null) {
               // It's at the edge of the map, where we don't have
               // the noisy edges computed. TODO: figure out how to
               // fill in these edges from the voronoi library.
               continue;
             }

             if (gradientFillProperty != null) {
               // We'll draw two triangles: center - corner0 -
               // midpoint and center - midpoint - corner1.
               var corner0:Corner = edge.v0;
               var corner1:Corner = edge.v1;

               // We pick the midpoint elevation/moisture between
               // corners instead of between polygon centers because
               // the resulting gradients tend to be smoother.
               var midpoint:Point = edge.midpoint;
               var midpointAttr:Number = 0.5*(corner0[gradientFillProperty]+corner1[gradientFillProperty]);
               drawGradientTriangle
                 (graphics,
                  new Vector3D(p.point.x, p.point.y, p[gradientFillProperty]),
                  new Vector3D(corner0.point.x, corner0.point.y, corner0[gradientFillProperty]),
                  new Vector3D(midpoint.x, midpoint.y, midpointAttr),
                  [colors.GRADIENT_LOW, colors.GRADIENT_HIGH], drawPath0);
               drawGradientTriangle
                 (graphics,
                  new Vector3D(p.point.x, p.point.y, p[gradientFillProperty]),
                  new Vector3D(midpoint.x, midpoint.y, midpointAttr),
                  new Vector3D(corner1.point.x, corner1.point.y, corner1[gradientFillProperty]),
                  [colors.GRADIENT_LOW, colors.GRADIENT_HIGH], drawPath1);
             } else {
               graphics.beginFill(color);
               drawPath0();
               drawPath1();
               graphics.endFill();
             }
           }
       }
  */
}


void MapGen::renderBridges( )
{
/*	var edge:Edge;

	for each (edge in map.edges) {
		if (edge.river > 0 && edge.river < 4
				&& !edge.d0.water && !edge.d1.water
				&& (edge.d0.elevation > 0.05 || edge.d1.elevation > 0.05)) {
			var n:Point = new Point(-(edge.v1.point.y - edge.v0.point.y), edge.v1.point.x - edge.v0.point.x);
			n.normalize(0.25 + (roads.road[edge.index]? 0.5 : 0) + 0.75*Math.sqrt(edge.river));
			graphics.lineStyle(1.1, colors.BRIDGE, 1.0, false, LineScaleMode.NORMAL, CapsStyle.SQUARE);
			graphics.moveTo(edge.midpoint.x - n.x, edge.midpoint.y - n.y);
			graphics.lineTo(edge.midpoint.x + n.x, edge.midpoint.y + n.y);
			graphics.lineStyle();
		}
	}*/
}


void MapGen::renderRoads( )
{
/*	// First draw the roads, because any other feature should draw
	// over them. Also, roads don't use the noisy lines.
	var p:Center, A:Point, B:Point, C:Point;
	var i:int, j:int, d:Number, edge1:Edge, edge2:Edge, edges:Vector.<Edge>;

	// Helper function: find the normal vector across edge 'e' and
	// make sure to point it in a direction towards 'c'.
	function normalTowards(e:Edge, c:Point, len:Number):Point {
		// Rotate the v0-->v1 vector by 90 degrees:
		var n:Point = new Point(-(e.v1.point.y - e.v0.point.y), e.v1.point.x - e.v0.point.x);
		// Flip it around it if doesn't point towards c
		var d:Point = c.subtract(e.midpoint);
		if (n.x * d.x + n.y * d.y < 0) {
			n.x = -n.x;
			n.y = -n.y;
		}
		n.normalize(len);
		return n;
	}

	for each (p in map.centers) {
		if (roads.roadConnections[p.index]) {
			if (roads.roadConnections[p.index].length == 2) {
				// Regular road: draw a spline from one edge to the other.
				edges = p.borders;
				for (i = 0; i < edges.length; i++) {
					edge1 = edges[i];
					if (roads.road[edge1.index] > 0) {
						for (j = i+1; j < edges.length; j++) {
							edge2 = edges[j];
							if (roads.road[edge2.index] > 0) {
								// The spline connects the midpoints of the edges
								// and at right angles to them. In between we
								// generate two control points A and B and one
								// additional vertex C.  This usually works but
								// not always.
								d = 0.5*Math.min
										(edge1.midpoint.subtract(p.point).length,
												edge2.midpoint.subtract(p.point).length);
								A = normalTowards(edge1, p.point, d).add(edge1.midpoint);
								B = normalTowards(edge2, p.point, d).add(edge2.midpoint);
								C = Point.interpolate(A, B, 0.5);
								graphics.lineStyle(1.1, colors['ROAD'+roads.road[edge1.index]]);
								graphics.moveTo(edge1.midpoint.x, edge1.midpoint.y);
								graphics.curveTo(A.x, A.y, C.x, C.y);
								graphics.lineStyle(1.1, colors['ROAD'+roads.road[edge2.index]]);
								graphics.curveTo(B.x, B.y, edge2.midpoint.x, edge2.midpoint.y);
								graphics.lineStyle();
							}
						}
					}
				}
			} else {
				// Intersection or dead end: draw a road spline from
				// each edge to the center
				for each (edge1 in p.borders) {
					if (roads.road[edge1.index] > 0) {
						d = 0.25*edge1.midpoint.subtract(p.point).length;
						A = normalTowards(edge1, p.point, d).add(edge1.midpoint);
						graphics.lineStyle(1.4, colors['ROAD'+roads.road[edge1.index]]);
						graphics.moveTo(edge1.midpoint.x, edge1.midpoint.y);
						graphics.curveTo(A.x, A.y, p.point.x, p.point.y);
						graphics.lineStyle();
					}
				}
			}
		}
	}*/
}


void MapGen::renderEdges( )
{
/*	var p:Center, r:Center, edge:Edge;

	for each (p in map.centers) {
		for each (r in p.neighbors) {
			edge = map.lookupEdgeFromCenter(p, r);
			if (noisyEdges.path0[edge.index] == null
					|| noisyEdges.path1[edge.index] == null) {
				// It's at the edge of the map
				continue;
			}
			if (p.ocean != r.ocean) {
				// One side is ocean and the other side is land -- coastline
				graphics.lineStyle(2, colors.COAST);
			} else if ((p.water > 0) != (r.water > 0) && p.biome != 'ICE' && r.biome != 'ICE') {
				// Lake boundary
				graphics.lineStyle(1, colors.LAKESHORE);
			} else if (p.water || r.water) {
				// Lake interior – we don't want to draw the rivers here
				continue;
			} else if (lava.lava[edge.index]) {
				// Lava flow
				graphics.lineStyle(1, colors.LAVA);
			} else if (edge.river > 0) {
				// River edge
				graphics.lineStyle(Math.sqrt(edge.river), colors.RIVER);
			} else {
				// No edge
				continue;
			}

			graphics.moveTo(noisyEdges.path0[edge.index][0].x,
					noisyEdges.path0[edge.index][0].y);
			drawPathForwards(graphics, noisyEdges.path0[edge.index]);
			drawPathBackwards(graphics, noisyEdges.path1[edge.index]);
			graphics.lineStyle();
		}
	}*/
}


void MapGen::renderDebugPolygons( )
{
/*	var p:Center, q:Corner, edge:Edge, point:Point, color:int;

	if (map.centers.length == 0) {
		// We're still constructing the map so we may have some points
		graphics.beginFill(0xdddddd);
		graphics.drawRect(0, 0, SIZE, SIZE);
		graphics.endFill();
		for each (point in map.points) {
			graphics.beginFill(0x000000);
			graphics.drawCircle(point.x, point.y, 1.3);
			graphics.endFill();
		}
	}

	for each (p in map.centers) {
		color = colors[p.biome] || (p.ocean? colors.OCEAN : p.water? colors.RIVER : 0xffffff);
		graphics.beginFill(interpolateColor(color, 0xdddddd, 0.2));
		for each (edge in p.borders) {
			if (edge.v0 && edge.v1) {
				graphics.moveTo(p.point.x, p.point.y);
				graphics.lineTo(edge.v0.point.x, edge.v0.point.y);
				if (edge.river > 0) {
					graphics.lineStyle(2, displayColors.RIVER, 1.0);
				} else {
					graphics.lineStyle(0, 0x000000, 0.4);
				}
				graphics.lineTo(edge.v1.point.x, edge.v1.point.y);
				graphics.lineStyle();
			}
		}
		graphics.endFill();
		graphics.beginFill(p.water > 0 ? 0x003333 : 0x000000, 0.7);
		graphics.drawCircle(p.point.x, p.point.y, 1.3);
		graphics.endFill();
		for each (q in p.corners) {
			graphics.beginFill(q.water? 0x0000ff : 0x009900);
			graphics.drawRect(q.point.x-0.7, q.point.y-0.7, 1.5, 1.5);
			graphics.endFill();
		}
	}*/
}


void MapGen::renderWatersheds()
{
/*	var edge:Edge, w0:int, w1:int;

	for each (edge in map.edges) {
		if (edge.d0 && edge.d1 && edge.v0 && edge.v1
				&& !edge.d0.ocean && !edge.d1.ocean) {
			w0 = watersheds.watersheds[edge.d0.index];
			w1 = watersheds.watersheds[edge.d1.index];
			if (w0 != w1) {
				graphics.lineStyle(3.5, 0x000000, 0.1*Math.sqrt((map.corners[w0].watershed_size || 1) + (map.corners[w1].watershed.watershed_size || 1)));
				graphics.moveTo(edge.v0.point.x, edge.v0.point.y);
				graphics.lineTo(edge.v1.point.x, edge.v1.point.y);
				graphics.lineStyle();
			}
		}
	}

	for each (edge in map.edges) {
		if (edge.river) {
			graphics.lineStyle(1.0, 0x6699ff);
			graphics.moveTo(edge.v0.point.x, edge.v0.point.y);
			graphics.lineTo(edge.v1.point.x, edge.v1.point.y);
			graphics.lineStyle();
		}
	}*/
}
