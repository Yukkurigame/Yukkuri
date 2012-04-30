/*
 * hacks.h
 *
 *  Created on: 18.03.2012
 */

#ifndef HACKS_H_
#define HACKS_H_

template<typename Vector>
static void clear_vector( Vector* V ){
	for( typename Vector:: iterator it = V->begin(), vend = V->end(); it != vend; ++it ){
		delete (*it);
	}
	V->clear();
}

//TODO: decltype
#define FOREACH( item, vec ) for( typeof((vec).begin()) item = (vec).begin(); \
										item != (vec).end(); ++item )
#define FOREACHIT( vec ) FOREACH( it, vec )

#endif /* HACKS_H_ */
