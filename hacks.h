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

template<typename Vector>
static void clear_cvector( Vector* V ){
	for( typename Vector:: iterator it = V->begin(), vend = V->end(); it != vend; ++it ){
		free(*it);
	}
	V->clear();
}

//TODO: decltype
#define FOREACH( item, vec ) for( typeof((vec).begin()) item = (vec).begin(), end##item = (vec).end(); \
										item != end##item; ++item )
#define FOREACHIT( vec ) FOREACH( it, vec )
#define FOREACHP( item, vec ) for( typeof((vec)->begin()) item = (vec)->begin(), end##item = (vec)->end(); \
										item != end##item; ++item )
#define FOREACHPIT( vec ) FOREACHP( it, vec )

#define GROW_ARRAY( array, type, req, size) 								\
	if( req > size ){ 														\
		size = req << 1;													\
		array = (type*)realloc( array, (unsigned)(size * sizeof(type)) );	\
	}

#endif /* HACKS_H_ */
