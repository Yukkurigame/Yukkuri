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


void Watersheds::createWatersheds( MapGenerator& map )
{
	// Find the lowest corner of the polygon, and set that as the
	// exit point for rain falling on this polygon
	FOREACHIT( map.centers ){
		Center* p = (*it);
		Corner* s = NULL;
		FOREACH( tit, p->corners ){
			Corner* q = (*tit);
			if( s == NULL || q->elevation < s->elevation )
				s = q;
		}
		lowestCorner[p->index] = (s == NULL) ? -1 : s->index;
		watersheds[p->index] = (s == NULL) ? -1 : (s->watershed == NULL) ? -1 : s->watershed->index;
	}
}
