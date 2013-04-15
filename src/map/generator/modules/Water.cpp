/*
 * Water.cpp
 *
 *  Created on: 13.04.2013
 */

#include "map/generator/modules/Water.h"
#include "debug.h"
#include "hacks.h"


namespace MapWater
{
	void redistributeMoisture( list< Corner* >& );
	void calculateWatersheds( );
	void createRivers( );
	void assignCornerMoisture( );
	void assignPolygonMoisture( );

	MapProto* proto;
	GeneratorModule module = {	&init, &clean, &process	};
}



void MapWater::init( )
{
	proto = NULL;
}


void MapWater::process( MapProto* pr )
{
	proto = pr;

	Debug::debug( Debug::MAP, "Assign moisture...\n" );

	// Determine watersheds: for every corner, where does it flow
	// out into the ocean?
	calculateWatersheds();

	// Create rivers.
	createRivers();

	// Determine moisture at corners, starting at rivers
	// and lakes, but not oceans. Then redistribute
	// moisture to cover the entire range evenly from 0.0
	// to 1.0. Then assign polygon moisture as the average
	// of the corner moisture.
	assignCornerMoisture();
	list< Corner* > cnrs = MapGenerator::landCorners( proto );
	redistributeMoisture( cnrs );
	assignPolygonMoisture();
}


void MapWater::clean( )
{
	proto = NULL;
}


GeneratorModule* MapWater::get_module()
{
	return &module;
}


bool compareMoisture( const Corner* a, const Corner* b )
{
	return a->moisture < b->moisture;
}


/**
 * Change the overall distribution of moisture to be evenly distributed.
 */
void MapWater::redistributeMoisture( list< Corner* >& locations )
{
	locations.sort( compareMoisture );
	float length = locations.count;
	int index = 0;
	ITER_LIST( Corner*, locations ){
		double m = (double)index / length;
		it->data->moisture = m;
		index++;
	}
}



/**
 * Calculate the watershed of every land point. The watershed is
 * the last downstream land point in the downslope graph.
 * TODO: watersheds are currently calculated on corners, but it'd
 * be more useful to compute them on polygon centers so that every
 * polygon can be marked as being in one watershed.
 */
void MapWater::calculateWatersheds( )
{

	// Initially the watershed pointer points downslope one step.
	Corner* q;
	FOREACH1( q, proto->corners ) {
		q->watershed = q;
		if( !q->ocean && !q->coast )
			q->watershed = q->downslope;
	}

	// Follow the downslope pointers to the coast. Limit to 100
	// iterations although most of the time with NUM_POINTS=2000 it
	// only takes 20 iterations because most points are not far from
	// a coast.  TODO: can run faster by looking at
	// p.watershed.watershed instead of p.downslope.watershed.
	for( int i = 0, changed = 0; i < 100; ++i, changed = 0 ){
		Corner* q;
		FOREACH1( q, proto->corners ) {
			if( !q->ocean && !q->coast && !q->watershed->coast ){
				Corner* r = q->downslope->watershed;
				//Corner* r = q->downslope->watershed;
				if( !r->ocean )
					q->watershed = r;
				changed++;
			}
		}
		if( !changed )
			break;
	}

	// How big is each watershed?
	FOREACH1( q, proto->corners ) {
		Corner* r = q->watershed;
		r->watershed_size++;
	}

	int min_watershed = 20;
	for( int i = 0, changed = 0; i < 100; ++i, changed = 0 ){
		FOREACH1( q, proto->corners ) {
			if( q->watershed->watershed_size > min_watershed )
				continue;
			ITER_LIST( Corner*, q->adjacent ){
				Corner* r = it->data->watershed;
				if( r->watershed_size > min_watershed ){
					q->watershed = r->watershed;
					changed = true;
					break;
				}
			}
		}
		if( !changed )
			break;
	}
}


/**
 * Create rivers along edges. Pick a random corner point, then
 * move downslope. Mark the edges and corners as rivers.
 */
void MapWater::createRivers( )
{
	int length = proto->corners.size();
	if( !length )
		return;

	list< Corner* > river_sources;

	for( int i = 0; i < GENERATOR_SIZE / 2; i++ ){
		// FIXME: this place prevents from usnig list for corners
		Corner* q = proto->corners[proto->mapRandom.nextInt( 0, length - 1 )];
		if( q->ocean || q->elevation < 0.3 || q->elevation > 0.9 )
			continue;

		river_sources.push(q);

		// Bias rivers to go west: if (q.downslope.x > q.x) continue;
		while( !q->coast ){
			if( q == q->downslope )
				break;
			Edge* edge = MapGenerator::lookupEdgeFromCorner( q, q->downslope );
			edge->river++;
			q->river++;
			q->downslope->river++; // TODO: fix double count
			q = q->downslope;
		}
	}

	// Make all rivers end at lakes or another rivers
	while( river_sources.count > 0 ){
		Corner* source = river_sources.pop();
		Corner* q = source;
		while( !q->coast && q != q->downslope && q->downslope->river )
			q = q->downslope;
		if( q->coast )
			continue;
		if( q->river > 3 )
			q->downslope->touches.head->data->water = true;
		// TODO: connect to another rivers
	}
}


/**
 * Calculate moisture. Freshwater sources spread moisture: rivers
 * and lakes (not oceans). Saltwater sources have moisture but do
 * not spread it (we set it at the end, after propagation).
 */
void MapWater::assignCornerMoisture( )
{
	list< Corner* > queue;

	// Fresh water
	Corner* q;
	FOREACH1( q, proto->corners ) {
		if( ( q->water || q->river > 0 ) && !q->ocean ){
			q->moisture =
					q->river > 0 ? std::min( 3.0, 0.2 * (double)q->river ) : 1.0;
			queue.push_back( q );
		}else{
			q->moisture = 0.0;
		}
	}

	while( queue.count > 0 ){
		Corner* q = queue.pop();
		ITER_LIST( Corner*, q->adjacent ){
			Corner* r = it->data;
			double newMoisture = q->moisture * 0.9;
			if( newMoisture > r->moisture ){
				r->moisture = newMoisture;
				queue.push_back( r );
			}
		}
	}
	// Salt water
	FOREACH1( q, proto->corners ) {
		if( q->ocean || q->coast )
			q->moisture = 1.0;
	}
}


/**
 * Polygon moisture is the average of the moisture at corners
 */
void MapWater::assignPolygonMoisture( )
{
	Center* p;
	FOREACH1( p, proto->centers ) {
		double sumMoisture = 0.0;
		double count = p->corners.count;
		ITER_LIST( Corner*, p->corners ){
			Corner* q = it->data;
			if( q->moisture > 1.0 )
				q->moisture = 1.0;
			sumMoisture += q->moisture;
		}
		p->moisture = sumMoisture / count;
	}
}
