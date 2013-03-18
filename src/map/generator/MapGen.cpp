/*
 * MapGen.cpp
 *
 *  Created on: 03.03.2013
 */

#include "map/generator/MapGen.h"
#include "graphics/Render.h"
#include "graphics/sprite/Mesh.h"
#include "graphics/utils/ElasticBox.h"
#include "graphics/render/GLTextures.h"
#include "graphics/render/Textures.h"
#include "graphics/render/VBuffer.h"
#include "utils/misc.h"
#include "hacks.h"
#include "debug.h"
#include <cmath>



MapGen::MapGen( )
{
	islandType = ifSquare;
	islandSeedInitial = "85882-1";
	map = new MapGenerator( SIZE );
}

MapGen::~MapGen( )
{
	// TODO Auto-generated destructor stub
}

void MapGen::newIsland( IslandForm type, const char* seed_string )
{
	int seed = 0;
	int variant = 0;

	Debug::debug( Debug::MAP, "Shaping map...\n" );

	if( !seed_string || seed_string[0] == '\0' ){
		seed_string = islandSeedInitial.c_str();
	}
	UINT len = strlen(seed_string);

	// Convert the string into a number. This is a cheesy way to
	// do it but it doesn't matter. It just allows people to use
	// words as seeds.
	for( UINT i = 0; i < len; ++i ){
		seed = ( seed << 4 ) | (UINT)seed_string[i];
	}
	seed %= 100000;
	variant = 1 + floor( 9.0 * random() );
	islandType = type;

	ElasticRectPODBox box = ElasticRectPODBox( );
	if( !box.calculate( SIZE, SIZE, 6 ) ){
		Debug::debug( Debug::MAP, "Cannot draw map." );
		return;
	}

	atlas.w = box.Width;
	atlas.h = box.Height;

	TextureProxy tp;
	{
		tp.id = seed_string;
		// Too static
		tp.cols = box.cols;
		tp.rows = box.rows;
		tp.abs.width = tp.cols * SIZE;
		tp.abs.height = tp.rows * SIZE;
		// Texture occupies all atlas
		tp.atlas.width = static_cast< float >( tp.abs.width )
				/ static_cast< float >( box.Width );
		tp.atlas.height = static_cast< float >( tp.abs.height )
				/ static_cast< float >( box.Height );
		tp.atlas.x = tp.atlas.y = 0.0;
	}
	GLTextures::generate( &atlas );
	texture_id = Textures::push( &tp, atlas.tex, 0 );

	map->newIsland( type, seed, variant );
}

void MapGen::go( )
{
	//roads = new Roads();
	//lava = new Lava();
	map->go( 0, 1 );
	map->go( 1, 2 );
	map->go( 2, 3 );
	//map->assignBiomes();

	Debug::debug( Debug::MAP, "Features...\n" );
	map->go( 3, 6 );
	map->assignBiomes();
	//drawMap();

	//roads.createRoads(map);
	// lava.createLava(map, map.mapRandom.nextDouble);
	watersheds.createWatersheds( map->centers );
	//noisyEdges.buildNoisyEdges( map->centers, /*lava,*/ &map->mapRandom );
	drawMap( gmWatersheds, gmWatersheds );
	drawMap( gmPolygons, gmPolygons );
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

void MapGen::drawMap( GeneratorMode mode, int picture )
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
			renderDebugPolygons( picture );
			break;
		case gmWatersheds:
			//noiseLayer.visible = false;
			renderDebugPolygons( picture );
			renderWatersheds( picture );
			return;
			break;
		case gmBiome:
			renderPolygons( picture );
			break;
		case gmSlopes:
			renderPolygons( picture );
			break;
		case gmSmooth:
			renderPolygons( picture );
			break;
		case gmElevation:
			renderPolygons( picture );
			break;
		case gmMoisture:
			renderPolygons( picture );
			break;
	}

	if( mode != gmSlopes && mode != gmMoisture )
		renderRoads( picture );

	if( mode != gmPolygons )
		renderEdges( picture );

	if( mode != gmSlopes && mode != gmMoisture )
		renderBridges( picture );
}


void MapGen::render3dPolygons(  )
{
/*	double zScale = -0.15*SIZE;

	if( sprite ){
		RenderManager::FreeGLSprite(sprite);
		sprite = NULL;
	}

	int count = 0;
	// Calculate count of vertices
	FOREACHIT1( map->centers ){
		count += (*it)->borders.count;
	}

	if( !count )
		return;

	int mesh = MeshManager::get("mesh_terrain");
	sprite = RenderManager::CreateGLSprite( 0, 0, 1, SIZE, SIZE, mesh );

	GLBrush& brush = sprite->brush;
	brush.init(-1);

	// Init vertex brush
	int vx_count = count * 3;
	brush.resize_verticles( vx_count );
	brush.indices_count = 0;
	brush.indices_list = (UINT*)malloc( (UINT)sizeof(UINT) * vx_count );
	VertexV2FT2FC4UI* arr = brush.points();

	Center* p;
	FOREACH1( p, map->centers ){
		ITER_LIST( Edge*, p->borders ){
			Edge* edge = it->data;
			UINT color = BiomesColors[p->biome];
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
*/
	// Render engine cares about all other stuff, not me
}


void MapGen::renderPolygons( int picture )
{
/*	Center* p;
	FILE *fp = fopen("lvt", "w");

	FOREACH1( p, map->centers ){
		//ITER_LIST( Corner*, p->corners ){
			//Center* r = it->data;
			//Corner* c = it->data;
			//Edge* edge = map->lookupEdgeFromCenter( p, r );
			//int color = BiomesColors[p->biome];

			//if( noisyEdges.path0.count(edge->index) < 1 ||
			//	noisyEdges.path1.count(edge->index) < 1 )
			//	continue;
			//list< s2f >* path0 = noisyEdges.path0[edge->index];
			//list< s2f >* path1 = noisyEdges.path1[edge->index];
			//fprintf( fp, " %f:%f", c->point.x, c->point.y );
			//ITER_LISTP( s2f, path0 ){
			//	fprintf( fp, " %f:%f", it->data.x, it->data.y );
			//}
			//fprintf( fp, " %f:%f\n",  p->point.x, p->point.y );
			//fprintf( fp, "line");
			//fprintf( fp, " %f:%f",  p->point.x, p->point.y );
			//ITER_LISTP( s2f, path1 ){
			//	fprintf( fp, " %f:%f", it->data.x, it->data.y );
			//}
			//fprintf( fp, " %f:%f\n",  p->point.x, p->point.y );
		//}
		fprintf( fp, "%f %f\n",  p->point.x, p->point.y );
	}
*/
}


void MapGen::renderBridges( int picture )
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


void MapGen::renderRoads( int picture )
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


void MapGen::renderEdges( int picture )
{
	list< VertexV2FT2FC4UI* > lines;

	Center* p;
	FOREACH1( p, map->centers ){
		ITER_LIST( Center*, p->neighbors ){
			Center* r = it->data;
			Edge* edge = map->lookupEdgeFromCenter( p, r );
			if( noisyEdges.path[0][edge->index] == NULL
					|| noisyEdges.path[1][edge->index] == NULL ){
				// It's at the edge of the map
				continue;
			}
			int thikness = 1;
			UINT color = gcBLACK;

			if( p->ocean != r->ocean ){
				// One side is ocean and the other side is land -- coastline
				color = gcCOAST;
				thikness = 2;
			}else if( ( p->water > 0 ) != ( r->water > 0 ) &&
					p->biome != bICE && r->biome != bICE){
				// Lake boundary
				color = gcLAKESHORE;
			}else if( p->water || r->water ){
				// Lake interior – we don't want to draw the rivers here
				continue;
			//}else if( lava.lava[edge.index] ){
				// Lava flow
				//	color = gcLAVA );
			}else if( edge->river > 0 ){
				// River edge
				thikness = sqrt( edge->river );
				color = gcRIVER;
			}else{
				// No edge
				continue;
			}

			prepareLine( edge, thikness, color, lines );
		}
	}

	draw( lines, picture );
}


void MapGen::renderDebugPolygons( int picture )
{
	list< VertexV2FT2FC4UI* > lines;

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
*/

	Center* p;
	FOREACH1( p, map->centers ){
		int color;
		if( p->biome != bNONE )
			color = BiomesColors[p->biome];
		else
			color = ( p->ocean ? gcOCEAN : ( p->water ? gcRIVER : gcRED ) );
		ITER_LIST( Edge*, p->borders ){
			Edge* edge = it->data;
			if( !edge->v0 || !edge->v1 )
				continue;

			prepareLine( edge, 1.0, interpolateColor(color, gcGRAY, 0.2), lines );

			/* Between points
			if (edge.river > 0) {
				graphics.lineStyle(2, displayColors.RIVER, 1.0);
			} else {
				graphics.lineStyle(0, 0x000000, 0.4);
			}*/
		}

		/*graphics.beginFill(p.water > 0 ? 0x003333 : 0x000000, 0.7);
		graphics.drawCircle(p.point.x, p.point.y, 1.3);
		graphics.endFill();
		for each (q in p.corners) {
			graphics.beginFill(q.water? 0x0000ff : 0x009900);
			graphics.drawRect(q.point.x-0.7, q.point.y-0.7, 1.5, 1.5);
			graphics.endFill();
		}*/
	}

	draw( lines, picture );
}


void MapGen::renderWatersheds( int picture )
{
	list< VertexV2FT2FC4UI* > lines;

	Edge* edge;
	FOREACH1( edge, map->edges ){
		if( !edge->v0 || !edge->v1 )
			continue;

		if( edge->d0 && edge->d1 && !edge->d0->ocean && !edge->d1->ocean ){
			int w0 = watersheds.watersheds[edge->d0->index];
			int w1 = watersheds.watersheds[edge->d1->index];
			if( w0 != w1 ){
				float a = 0.5 * sqrt(
					( map->corners[w0]->watershed_size || 1 )
					+ ( map->corners[w1]->watershed->watershed_size	|| 1 ) );
				prepareLine( edge, 3.5, gcBLACK, lines, a * 255 );
			}
		}

		if( edge->river )
			prepareLine( edge, 1.0, gcWATERSHEDS, lines );
	}

	draw( lines, picture );
}


void calculate_line( const s2f* p[2], float thikness, UINT color,
		list< VertexV2FT2FC4UI* >& lines, int alpha = 255 )
{
	float angle = atan2( p[1]->y - p[0]->y, p[1]->x - p[0]->x );
	float sina = thikness / 2.0 * sin( angle );
	float cosa = thikness / 2.0 * cos( angle );
	float pts[12] = { p[0]->x + sina, p[0]->y - cosa,
					  p[1]->x + sina, p[1]->y - cosa,
					  p[1]->x - sina, p[1]->y + cosa,
					  p[1]->x - sina, p[1]->y + cosa,
					  p[0]->x - sina, p[0]->y + cosa,
					  p[0]->x + sina, p[0]->y - cosa, };

	for( int i = 0; i < 12; i += 2 ){
		VertexV2FT2FC4UI* point = new VertexV2FT2FC4UI;
		point->verticles.x = pts[i];
		point->verticles.y = pts[i + 1];
		point->color.set( color );
		lines.push_back( point );
	}
}

void MapGen::prepareLine( Edge* edge, float thikness, UINT color,
		list< VertexV2FT2FC4UI* >& lines, int alpha )
{
	const s2f* line[2] = { &edge->v0->point, &edge->v1->point };
	calculate_line( line, thikness, color, lines, alpha );

	// Noisy edges. Works bad.
/*	for( int i = 0; i < 2; ++i ){
		list < s2f >* path = noisyEdges.path[i][edge->index];
		ITER_LISTP( s2f, path ) {
			if( !it->next )
				continue;

			const s2f* p[2] = { &it->data, &it->next->data };

			//int segments = 1;
			//if( (!i && !it->next->next) || (i && it == path->head ) )
			//	segments = 2;

			// Iterate 2 times because we using 2 points at once
			it = it->next;

			prepare_point( p, thikness, color, lines, triangles );

		}
	}*/
}


void MapGen::draw( list< VertexV2FT2FC4UI* >& verticles, int picture )
{
	UINT VBOHandle;
	VBuffer::create( &VBOHandle );
	Sprite* sprite = new Sprite(VBOHandle);
	sprite->setTexture(0);

	GLBrush& brush = sprite->brush;

	int vx_count = verticles.count;
	brush.resize_verticles( vx_count );
	brush.resize_indices( vx_count );

	VertexV2FT2FC4UI* arr = brush.points();

	s2f offset;
	TextureInfo* tex = Textures::get_pointer( texture_id );
	tex->getTexturePosition( offset, picture );

	int index = 0;
	ITER_LIST( VertexV2FT2FC4UI*, verticles ){
		VertexV2FT2FC4UI* vel = it->data;
		VertexV2FT2FC4UI* vpt = &arr[index];
		vpt->verticles.x = offset.x + vel->verticles.x;
		vpt->verticles.y = offset.y + vel->verticles.y;
		vpt->color.set( vel->color );
		brush.indices_list[index] = brush.point_index + index;
		index++;
	}

	GLTextures::update( &atlas, sprite );

	delete sprite;
	VBuffer::free_buffer( &VBOHandle );
}
