/*
 * mesh.h
 *
 *  Created on: 02.02.2013
 */

#ifndef MESH_H_
#define MESH_H_

#include "graphics/sprite/Brush.h"


namespace MeshManager {

	void init( );
	void clean( );
	int get( const char* name );
	void load( GLBrush* brush );

}



#endif /* MESH_H_ */
