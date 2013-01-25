/*
 * gl_uniforms.h
 *
 *  Created on: 25.01.2013
 *
 */

#ifndef GL_UNIFORMS_H_
#define GL_UNIFORMS_H_

#include "types.h"

enum UniformTypes {
	glu1i,
	glu2fv,
	glu3fv,
	gluMat4fv,
};


struct UniformData {
	char* name;
	enum UniformTypes type;
	void* data;
};

struct UniformHandler {
	int location;
	UINT index;
};

struct UniformHandlers {
	UINT count;
	UniformHandler* handlers;
};


namespace UniformsManager {

	UINT register_uniform( const char* name, enum UniformTypes type );
	void pass_data( UINT id, int data );
	void pass_data( UINT id, float* data );
	void send_data( int location, UINT id, int count = 1 );

}


#endif /* GL_UNIFORMS_H_ */
