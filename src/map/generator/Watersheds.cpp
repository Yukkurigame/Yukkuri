/*
 * Watersheds.cpp
 *
 *  Created on: 02.03.2013
 */

#include "Watersheds.h"
#include "hacks.h"

Watersheds::Watersheds( )
{
	// TODO Auto-generated constructor stub

}

Watersheds::~Watersheds( )
{
	// TODO Auto-generated destructor stub
}


void Watersheds::createWatersheds( std::vector< Center* >& centers )
{
	// Find the lowest corner of the polygon, and set that as the
	// exit point for rain falling on this polygon
	Center* p;
	FOREACH1( p, centers ){
		Corner* s = 0;
		ITER_LIST( Corner*, p->corners ){
			Corner* q = it->data;
			if( s == 0 || q->elevation < s->elevation )
				s = q;
		}
		lowestCorner[p->index] = (s == 0) ? -1 : s->index;
		watersheds[p->index] = (s == 0) ? -1 : (s->watershed == 0) ? -1 : s->watershed->index;
	}
}
