/*
 * VBOArray.cpp
 *
 *  Created on: 05.12.2012
 */


#include "graphics/utils/VBOArray.h"
#include "graphics/GraphicsTypes.h"
#include <cstring>
#include "debug.h"

// Critical length for last free block. All following blocks will be freed.
#define MAX_LAST_BLOCK 255

static const int chunk_size = sizeof(VertexV2FT2FC4UI);


/*	Change size of array
 * 	size - desired size
 */
void VBOArray::realloc_array( int size )
{
	int new_size = (array_size + size);
	if( new_size < 0 )
		new_size = 0;
	array = (VertexV2FT2FC4UI*)realloc(array, chunk_size * new_size );
	array_size = new_size;
}



/*	Search free space in array
 * 	size - desired size of block
 * 	returns first index of block if space found or -1 otherwise
 */
int VBOArray::search_space( unsigned int size )
{
	int index = -1;
	listElement<free_block*>* fb = free_space.head;
	while( fb ){
		free_block* block = fb->data;
		if( block->count >= size ){
			index = block->start;
			block->start += size;
			block->count -= size;
			if( block->count <= 0 )
				free_space.remove( fb );
			break;
		}
		fb = fb->next;
	}
	return index;
}


/*	Check two free_block to first overlap second
 *  returns -1 if no overlap between them
 *  returns count of elements right exceed origin.
 */
int VBOArray::right_overlap( free_block* origin, free_block* right ){
	if( origin == NULL || right == NULL )
		return -1;
	if( origin->start > right->start ){
		Debug::debug( Debug::GRAPHICS, "VBOArray: right block preceding origin. Overlap check canceled.\n" );
		return -1;
	}
	unsigned int end = origin->start + origin->count;
	unsigned int rend = right->start + right->count;
	if( right->start > end )	// No overlap
		return -1;
	if( end < rend )			// Overlap on rend - end
		return rend - end;
	return 0;
}


//////////////////////////
// PUBLIC

void VBOArray::init()
{
	array = NULL;
	array_size = 0;
	free_space.init();
}


void VBOArray::clear()
{
	listElement<free_block*>* t = free_space.head;
	while( t != NULL ){
		delete t->data;
		t = t->next;
	}
	free_space.clear();
	free( array );
}



/*	Search in array for space. Allocate new if no space found
 * 	size - desired size
 * 	returns first index of new block
 */
int VBOArray::getSpace( unsigned int size )
{
	if( array == NULL ){
		realloc_array( size );
		return 0;
	}
	int index = search_space( size );
	if( index < 0 ){
		// No free space available. Reallocate array.
		index = array_size;
		realloc_array( size );
	}
	return index;
}


/*	Return unneeded space to array control. Makes it available for new allocations.
 * 	index - first index of block
 * 	count - size of block
 */
void VBOArray::freeSpace( unsigned int index, unsigned int count )
{
	if( count < 1 )
		return;
	if( (index + count) > array_size ){
		Debug::debug( Debug::GRAPHICS, "Attempt to free more space than available.\n" );
		return;
	}
	listElement<free_block*>* left = free_space.head;
	listElement<free_block*>* right = NULL;
	while( left != NULL ){
		if( index < left->data->start ){ 		// memory before any block.
			right = left;
			left = NULL;
			break;
		}else if( left->next == NULL ||			// only one left block exist
			index < left->next->data->start ){	// memory in or after left block
			// memory before right block
			right = left->next;
			break;
		}
		left = left->next;
	}
	int end = index + count;
	free_block* block = NULL;
	if( left != NULL &&
		( left->data->start + left->data->count >= index ) ){	// Overlap with left
		int over = end - (left->data->start + left->data->count);
		if( over > 0 )
			left->data->count += over;
	}else{												// Follow left
		block = new free_block( index, count );
		free_space.insert( block, left );
	}
	// Check for overlaping with right
	if( !right )
		return;
	int exceed = right_overlap( block, right->data );
	listElement<free_block*>* next = NULL;
	while( exceed >= 0 ){
		next = right->next;
		free_space.remove( right );
		right = NULL;
		block->count += exceed;
		if( !next )
			break;
		right = next;
		exceed = right_overlap( block, next->data );
	}
	// Search for large end block
	while( right != NULL && right->next != NULL )
		right = right->next;
	if( right != NULL ){
		// If last block is big enough remove it
		if( right->data->start + right->data->count == array_size &&
			right->data->count > MAX_LAST_BLOCK ){
			realloc_array( - right->data->count );
			free_space.remove( right );
		}
	}
}


/*  Return pointer to index in array.
 *  Index may point to free space with random data.
 */
VertexV2FT2FC4UI* VBOArray::pointer( unsigned int index )
{
	if( (int)index >= array_size )
		return NULL;
	return &array[index];
}
