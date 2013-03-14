/*
 * hacks.h
 *
 *  Created on: 18.03.2012
 */

#ifndef HACKS_H_
#define HACKS_H_

#ifdef _MSC_VER
	#define typeof decltype
#endif

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


#define FOREACH( item, iterator, vec ) \
	for( typeof((vec).begin()) iterator = (vec).begin(), end##iterator = (vec).end(); \
		iterator != end##iterator && ( (item = *iterator) || 1 ); ++iterator )
#define FOREACH1( item, vec ) FOREACH( item, fit1, vec )
#define FOREACH2( item, vec ) FOREACH( item, fit2, vec )
#define FOREACH3( item, vec ) FOREACH( item, fit3, vec )

#define FOREACHIT1( vec ) \
	for( typeof((vec).begin()) it = (vec).begin(), end##it = (vec).end(); \
			it != end##it; ++it )
#define FOREACHIT2( vec ) \
	for( typeof((vec).begin()) it2 = (vec).begin(), end##it2 = (vec).end(); \
			it2 != end##it2; ++it2 )

#define FOREACHP( item, vec ) \
	for( typeof((vec)->begin()) item = (vec)->begin(), end##item = (vec)->end(); \
				item != end##item; ++item )
#define FOREACHPIT( vec ) FOREACHP( it, vec )

#define GROW_ARRAY( array, type, req, size) 								\
	if( req > size ){ 														\
		size = req << 1;													\
		array = (type*)realloc( array, (unsigned)(size * sizeof(type)) );	\
	}

#endif /* HACKS_H_ */
