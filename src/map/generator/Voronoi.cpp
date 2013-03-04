/*
 * Voronoi.cpp
 *
 *  Created on: 02.03.2013
 */

#include "Voronoi.h"
#include "3rdparty/voronoi/VoronoiDiagramGenerator.h"
#include "hacks.h"

static VoronoiDiagramGenerator vdg;

Voronoi::Voronoi( const std::vector<s2f>& points, const rect2f& size, int dist ) : sites(NULL)
{
	addSites( points );
	vdg.generateVoronoi( sites, sites_count, size.x, size.width,
						 size.y, size.height, dist );

	vdg.resetIterator();
	float x1, x2, y1, y2;
	while( vdg.getNext( x1, y1, x2, y2 ) )
		Lines.push_back( Line( x1, y1, x2, y2 ) );

}

Voronoi::~Voronoi( )
{

}

void Voronoi::addSites( const std::vector<s2f>& points )
{
	sites_count = points.size();
	sitesByLocation.clear();
	sites = (SourcePoint*)realloc( sites, sites_count * sizeof(SourcePoint) );
	for( UINT i = 0; i < sites_count; ++i ){
		const s2f* point = &points[i];
		SourcePoint* p = &sites[i];
		p->id = i;
		p->x = point->x;
		p->y = point->y;
		p->weight = random() * 100;
		sitesByLocation[point] = p;
	}
}

const std::vector<Line>& Voronoi::lines( )
{
	return Lines;
}
