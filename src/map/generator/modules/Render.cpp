/*
 * MapGen.cpp
 *
 *  Created on: 03.03.2013
 */

#include "map/generator/modules/Render.h"
#include "map/generator/Constants.h"
#include "map/generator/NoisyEdges.h"
#include "map/generator/Watersheds.h"
#include "map/generator/Histogram.h"
#include "graphics/GraphicsTypes.h"

#include "graphics/Render.h"
#include "graphics/sprite/Mesh.h"
#include "graphics/utils/ElasticBox.h"
#include "graphics/render/GLTextures.h"
#include "graphics/render/Textures.h"
#include "graphics/render/VBuffer.h"
#include "graphics/utils/Primitives.h"
#include "utils/misc.h"
#include "hacks.h"
#include "debug.h"
#include <cmath>


namespace MapRender
{
	int prepareTexture( );

	// Show some information about the maps
	void computeHistogram( float** hs, int* count, bucketFn fn );
	void drawHistogram( float x, float y, bucketFn fn, colorFn cfn, float width,
			float height, list< VertexV2FT2FC4UI* >* lines );
	void drawDistribution( float x, float y, bucketFn fn, colorFn cfn, float width,
			float height, list< VertexV2FT2FC4UI* >* lines );
	void drawHistograms( int picture );

	// Draw the map in the current map mode
	void drawMap( GeneratorMode, int picture );

	// 3D rendering of polygons. If the 'triangles3d' array is empty,
	// it's filled and the graphicsData is filled in as well. On
	// rendering, the triangles3d array has to be z-sorted and then
	// the resulting polygon data is transferred into graphicsData
	// before rendering.

	// Render the interior of polygons
	void renderPolygons( int picture );
	void renderGradientPolygions( int picture, UINT color_low, UINT color_high,
			NodeProperty, float min = 0.0, float max = 1.0 );

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

	void prepareLine( Edge* edge, float thikness, UINT color,
			list< VertexV2FT2FC4UI* >* lines, int alpha = 255 );
	void preparePolygon( Center* p, Edge* edge, UINT color,
			list< VertexV2FT2FC4UI* >* lines, int alpha = 255 );
	void draw( list< VertexV2FT2FC4UI* >& verticles, int picture );

	// Helper function for drawing triangles with gradients. This
	// function sets up the fill on the graphics object, and then
	// calls fillFunction to draw the desired path.
	//void drawGradientTriangle( s3f v1, s3f v2, s3f v3, colors[] colors:Array, fillFunction:Function)

	// Island shape is controlled by the islandRandom seed and the
	// type of island. The islandShape function uses both of them to
	// determine whether any point should be water or land.
	IslandForm islandType;

	//Roads roads;
	//Lava lava
	Watersheds watersheds;
	NoisyEdges noisyEdges;

	MapProto* proto = NULL;
	GeneratorModule module = {	&init, &clean, &process	};
}


void MapRender::init( )
{
	proto = NULL;
}


void MapRender::clean( )
{
	// We need to clean texture after generation.
	proto = NULL;
}


void MapRender::process( MapProto* pr )
{
	proto = pr;
	if( !prepareTexture() )
		return;

	watersheds.createWatersheds( proto->centers );

	drawMap( gmBiome, 0 );
	drawMap( gmWatersheds, 1 );
	drawMap( gmPolygons, 2 );
	drawMap( gmMoisture, 3 );
	drawMap( gmElevation, 4 );
	drawMap( gmTemperature, 5 );
}


GeneratorModule* MapRender::get_module()
{
	return &module;
}



int MapRender::prepareTexture( )
{
	proto->atlas.tex = 0;
	proto->texture_id = 0;

	ElasticRectPODBox box = ElasticRectPODBox();
	if( !box.calculate( GENERATOR_SIZE, GENERATOR_SIZE, 6 ) ){
		Debug::debug( Debug::MAP, "Cannot draw map." );
		return 0;
	}

	proto->atlas.w = box.Width;
	proto->atlas.h = box.Height;

	TextureProxy tp;
	{
		tp.id = strdup( proto->initial_seed_string );
		// Too static
		tp.cols = box.cols;
		tp.rows = box.rows;
		tp.abs.width = tp.cols * GENERATOR_SIZE;
		tp.abs.height = tp.rows * GENERATOR_SIZE;
		// Texture occupies all atlas
		tp.atlas.width = static_cast< float >( tp.abs.width )
				/ static_cast< float >( box.Width );
		tp.atlas.height = static_cast< float >( tp.abs.height )
				/ static_cast< float >( box.Height );
		tp.atlas.x = tp.atlas.y = 0.0;
	}
	GLTextures::generate( &proto->atlas );
	proto->texture_id = Textures::push( &tp, proto->atlas.tex, 0 );
	free( tp.id );

	return 1;
}



void MapRender::computeHistogram( float** hs, int* count, bucketFn fn )
{
	int size = proto->centers.size();
	*count = size;

	if( !size )
		return;

	float* histogram = new float[size];
	memset( &histogram[0], 0, sizeof(float) * size );

	Center* p;
	FOREACH1( p, proto->centers ) {
		int bucket = fn( p );
		if( bucket >= 0 )
			histogram[bucket] = ( histogram[bucket] || 0 ) + 1;
	}

	*hs = histogram;
}

void MapRender::drawHistogram( float x, float y, bucketFn fn, colorFn cfn,
		float width, float height, list< VertexV2FT2FC4UI* >* lines )
{
	float* histogram = NULL;
	int hcount = 0;
	computeHistogram( &histogram, &hcount, fn );
	if( !hcount ){
		delete[] histogram;
		return;
	}

	float scale = 0.0;
	for( int i = 0; i < hcount; ++i )
		scale = ( scale >= histogram[i] ? scale : histogram[i] );


	for( int i = 0; i < hcount; ++i ){
		if( histogram[i] ){
			float ratio = width / (float)hcount;
			GLPrimitives::prepare_rectangle( x + i * ratio, y + height,
					(ratio - 1.0 > 0.0 ? ratio - 1.0 : 0.0),
					-height * histogram[i] / scale,
					cfn(i), lines );
		}
	}

	delete[] histogram;
}

void MapRender::drawDistribution( float x, float y, bucketFn fn, colorFn cfn,
		float width, float height, list< VertexV2FT2FC4UI* >* lines )
{
	float* histogram = NULL;
	int hcount = 0;
	computeHistogram( &histogram, &hcount, fn );
	if( !hcount )
		return;
	float scale = 0.0;
	for( int i = 0; i < hcount; ++i )
		scale += histogram[i];

	for( int i = 0; i < hcount; ++i ){
		if( histogram[i] ){
			float w = histogram[i] / scale * width;
			GLPrimitives::prepare_rectangle( x, y,
					(w - 1.0 > 0.0 ? w - 1.0 : 0.0), height, cfn(i), lines );
			x += w;
		}
	}

	delete[] histogram;
}


void MapRender::drawHistograms( int picture )
{
	list< VertexV2FT2FC4UI* > lines;

	// There are pairs of functions for each chart. The bucket
	// function maps the polygon Center to a small int, and the
	// color function maps the int to a color.

	float x = 23.0;
	float y = 140.0;
	float width = 154.0;

	drawDistribution( x, y, &landTypeBucket, &landTypeColor, width, 20, &lines );
	drawDistribution( x, y + 25, &biomeBucket, &biomeColor, width, 20, &lines );
	drawHistogram( x, y + 55, &elevationBucket, &elevationColor, width, 30, &lines );
	drawHistogram( x, y + 95, &moistureBucket, &moistureColor, width, 20, &lines );

	draw( lines, picture );
	CLEAR_PTR_LIST( lines );
}


void MapRender::drawMap( GeneratorMode mode, int picture )
{
	//noiseLayer.visible = true;

	switch( mode ){
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
			renderGradientPolygions( picture, gcELEVATION_LOW, gcELEVATION_HIGH, gnpElevation );
			break;
		case gmMoisture:
			renderGradientPolygions( picture, gcMOISTURE_LOW, gcMOISTURE_HIGH, gnpMoisture );
			break;
		case gmTemperature:
			renderGradientPolygions( picture, gcTEMPERATURE_LOW, gcTEMPERATURE_HIGH,
					gnpTemperature, -60.5, 45.0 );
			break;
	}

	if( mode != gmSlopes && mode != gmMoisture )
		renderRoads( picture );

	if( mode != gmPolygons )
		renderEdges( picture );

	if( mode != gmSlopes && mode != gmMoisture )
		renderBridges( picture );

	if( mode != gmBiome )
		drawHistograms( picture );
}


void MapRender::renderPolygons( int picture )
{
	// My Voronoi polygon rendering doesn't handle the boundary
	// polygons, so I just fill everything with ocean first.
	//graphics.beginFill( colors.OCEAN );
	//graphics.drawRect( 0, 0, SIZE, SIZE );
	//graphics.endFill();

	list< VertexV2FT2FC4UI* > lines;

	Center* p;
	FOREACH1( p, proto->centers ) {
		ITER_LIST( Center*, p->neighbors ) {
			Center* r = it->data;
			Edge* edge = MapGenerator::lookupEdgeFromCenter( p, r );
			UINT color = BiomesColors[p->biome];
			preparePolygon( p, edge, color, &lines );
		}
	}

	draw( lines, picture );
	CLEAR_PTR_LIST( lines );
}

void MapRender::renderGradientPolygions( int picture, UINT color_low, UINT color_high, NodeProperty prop,
		float min, float max )
{
	list< VertexV2FT2FC4UI* > lines;

	Center* p;
	FOREACH1( p, proto->centers ) {
		ITER_LIST( Center*, p->neighbors ) {
			Center* r = it->data;
			Edge* edge = MapGenerator::lookupEdgeFromCenter( p, r );

			if( !edge->v0 || !edge->v1 )
				continue;

			/* I don't think this trash is useful here.
			list< s2f >* path[2];
			if( noisyEdges.processed() && (
					( path[0] = noisyEdges.path[0][edge->index] ) == NULL ||
					( path[1] = noisyEdges.path[1][edge->index] ) == NULL )
			)
				continue;
			*/

			// We'll draw two triangles: center - corner0 -
			// midpoint and center - midpoint - corner1.
			Corner* corner0 = edge->v0;
			Corner* corner1 = edge->v1;

			// Normalize properties
			float prop0 = fabs( corner0->property( prop ) - min ) / ( max - min );
			float prop1 = fabs( corner1->property( prop ) - min ) / ( max - min );

			// We pick the midpoint elevation/moisture between
			// corners instead of between polygon centers because
			// the resulting gradients tend to be smoother.
			const s2f& midpoint = edge->midpoint;
			UINT midpoint_color = interpolateColor( color_low, color_high, 0.5 * ( prop0 + prop1 ) );
			UINT p_color = interpolateColor( color_low, color_high,
					fabs( p->property( prop ) - min ) / ( max - min ) );

			float points[12] = {
					p->point.x, p->point.y,
					corner0->point.x, corner0->point.y,
					midpoint.x, midpoint.y,
					p->point.x, p->point.y,
					midpoint.x, midpoint.y,
					corner1->point.x, corner1->point.y,
			};
			UINT colors[6] = {
					p_color,
					interpolateColor( color_low, color_high, prop0 ),
					midpoint_color,
					p_color,
					midpoint_color,
					interpolateColor( color_low, color_high, prop1 )
			};

			GLPrimitives::prepare_triangles( points, 12, colors, &lines );
		}
	}

	draw( lines, picture );
	CLEAR_PTR_LIST( lines );
}


void MapRender::renderBridges( int picture )
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
void MapRender::renderRoads( int picture )
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


void MapRender::renderEdges( int picture )
{
	list< VertexV2FT2FC4UI* > lines;

	Center* p;
	FOREACH1( p, proto->centers ){
		ITER_LIST( Center*, p->neighbors ){
			Center* r = it->data;
			Edge* edge = MapGenerator::lookupEdgeFromCenter( p, r );

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

			prepareLine( edge, thikness, color, &lines );
		}
	}

	draw( lines, picture );
	CLEAR_PTR_LIST( lines );
}


void MapRender::renderDebugPolygons( int picture )
{
	list< VertexV2FT2FC4UI* > lines;

	Center* p;
	FOREACH1( p, proto->centers ){
		int color;
		if( p->biome != bNONE )
			color = BiomesColors[p->biome];
		else
			color = ( p->ocean ? gcOCEAN : ( p->water ? gcRIVER : gcRED ) );
		ITER_LIST( Edge*, p->borders ){
			Edge* edge = it->data;
			if( !edge->v0 || !edge->v1 )
				continue;

			preparePolygon( p, edge, interpolateColor(color, gcGRAY, 0.2), &lines );

			if( edge->river )
				prepareLine( edge, 1.8, gcRIVER, &lines );
			else
				prepareLine( edge, 0.7, gcBLACK, &lines );
		}

		UINT ccolor = ( p->water > 0 ? gcAQUA : gcBLACK );
		GLPrimitives::prepare_circle( p->point.x, p->point.y, 1.3, ccolor, &lines, 0.7);

		ITER_LIST( Corner*, p->corners ){
			Corner* q = it->data;
			UINT rcolor = q->water ? gcBLUE : gcDARKGREEN;
			GLPrimitives::prepare_rectangle( q->point.x - 0.7, q->point.y - 0.7,
					1.5, 1.5, rcolor, &lines );
		}
	}

	draw( lines, picture );
	CLEAR_PTR_LIST( lines );
}


void MapRender::renderWatersheds( int picture )
{
	list< VertexV2FT2FC4UI* > lines;

	// TODO: prevents corners from being list

	Edge* edge;
	FOREACH1( edge, proto->edges ){
		if( !edge->v0 || !edge->v1 )
			continue;

		if( edge->d0 && edge->d1 && !edge->d0->ocean && !edge->d1->ocean ){
			int w0 = watersheds.watersheds[edge->d0->index];
			int w1 = watersheds.watersheds[edge->d1->index];
			if( w0 != w1 ){
				float a = 0.5 * sqrt(
					( proto->corners[w0]->watershed_size || 1 )
					+ ( proto->corners[w1]->watershed->watershed_size	|| 1 ) );
				prepareLine( edge, 3.5, gcBLACK, &lines, a * 255 );
			}
		}

		if( edge->river )
			prepareLine( edge, 1.0, gcWATERSHEDS, &lines );
	}

	draw( lines, picture );
	CLEAR_PTR_LIST( lines );
}


void MapRender::prepareLine( Edge* edge, float thikness, UINT color,
		list< VertexV2FT2FC4UI* >* lines, int alpha )
{
	// It's at the edge of the map, where we don't have
	// the noisy edges computed. TODO: figure out how to
	// fill in these edges from the voronoi library.
	list< s2f >* path[2];
	if( noisyEdges.processed() && (
			( path[0] = noisyEdges.path[0][edge->index] ) == NULL ||
			( path[1] = noisyEdges.path[1][edge->index] ) == NULL )
	)
		return;

	float line[4] = {
		edge->v0->point.x, edge->v0->point.y,
		edge->v1->point.x, edge->v1->point.y
	};
	GLPrimitives::prepare_line( line, thikness, color, lines, alpha );

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

void MapRender::preparePolygon( Center* p, Edge* edge, UINT color,
				list< VertexV2FT2FC4UI* >* lines, int alpha )
{
	// It's at the edge of the map, where we don't have
	// the noisy edges computed. TODO: figure out how to
	// fill in these edges from the voronoi library.
	list< s2f >* path[2];
	if( noisyEdges.processed() && (
			( path[0] = noisyEdges.path[0][edge->index] ) == NULL ||
			( path[1] = noisyEdges.path[1][edge->index] ) == NULL )
	)
		return;

	if( !edge->v0 || !edge->v1 )
		return;

	float pts[6] = {
			p->point.x, p->point.y,
			edge->v0->point.x, edge->v0->point.y,
			edge->v1->point.x, edge->v1->point.y
	};

	GLPrimitives::prepare_triangles( pts, 6, color, lines );

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


void MapRender::draw( list< VertexV2FT2FC4UI* >& verticles, int picture )
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
	TextureInfo* tex = Textures::get_pointer( proto->texture_id );
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

	GLTextures::update( &proto->atlas, sprite );

	delete sprite;
	VBuffer::free_buffer( &VBOHandle );
}
