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




#endif /* HACKS_H_ */
