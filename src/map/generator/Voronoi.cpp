/*
 * Voronoi.cpp
 *
 *  Created on: 02.03.2013
 */

#include "Voronoi.h"
#include "3rdparty/voronoi/VoronoiDiagramGenerator.h"
#include "3rdparty/voronoi/EdgeReorderer.h"
#include "hacks.h"

static VoronoiDiagramGenerator vdg;

Voronoi::Voronoi( const std::vector<s2f>& points, const rect2f& size, int dist ) : sites(NULL)
{
	addSites( points );
	vdg.init();
	vdg.generateVoronoi( sites, sites_count, size.x, size.width,
						size.y, size.height, dist );
	bounds[ssLeft] = size.x;
	bounds[ssRight] = size.width;
	bounds[ssBottom] = size.y;
	bounds[ssTop] = size.height;
}

Voronoi::~Voronoi( )
{
	vdg.clean();
	if(sites)
		free(sites);
}

void Voronoi::egdes( VoronoiEdge*** el, int* size )
{
	vdg.getEdges( el, size );
}


void Voronoi::addSites( const std::vector<s2f>& points )
{
	sites_count = points.size();
	sitesByLocation.clear();
	sites = (SourcePoint*)realloc( sites, sites_count * (UINT)sizeof(SourcePoint) );
	for( UINT i = 0; i < sites_count; ++i ){
		const s2f* point = &points[i];
		SourcePoint* p = &sites[i];
		p->id = i;
		p->x = point->x;
		p->y = point->y;
		p->weight = random() * 100;
		sitesByLocation[point->x][point->y] = p;
	}
}

void Voronoi::region( const s2f& p, Point**& pts, int& count )
{
	SourcePoint* site = NULL;
	if( sitesByLocation.count( p.x ) > 0 && sitesByLocation[p.x].count( p.y ) > 0 )
		site = sitesByLocation[p.x][p.y];

	if( !site )
		return;

	Site* s = vdg.getSite( site->id );
	if( s ){
		site_region( s, bounds );
		count = s->region_size;
		pts = s->region;
	}
}


