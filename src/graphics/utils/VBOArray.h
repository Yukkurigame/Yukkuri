/*
 * VBOArray.h
 *
 *  Created on: 05.12.2012
 */

#ifndef VBOARRAY_H_
#define VBOARRAY_H_

#include "graphics/GraphicsTypes.h"
#include "utils/list.h"


class VBOArray
{
	struct free_block {
		unsigned int start;
		unsigned int count;
		free_block( unsigned int s, unsigned int c ) : start(s), count(c) {}
	};

	static int right_overlap( free_block* origin, free_block* right );
	void realloc_array( int size );
	int search_space( unsigned int size );


public:
	VBOArray() { init(); }
	~VBOArray() { clear(); }

	void init();
	void clear();

	int getSpace( unsigned int size );
	void freeSpace( unsigned int index, unsigned int count );
	VertexV2FT2FC4UI* pointer( unsigned int index );

	/* Returns pointer to first array element */
	inline VertexV2FT2FC4UI* head( ){ return pointer(0); }
	/* Returns count of array elements */
	inline int count( ) { return array_size; }
	/* Returns size of array in bytes */
	inline int size( ){ return array_size * sizeof(VertexV2FT2FC4UI); }


private:
	VertexV2FT2FC4UI* array;
	unsigned int array_size;
	list<free_block*> free_space;


};


#endif /* VBOARRAY_H_ */
