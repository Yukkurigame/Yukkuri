/*
 * list.h
 *
 *  Created on: 24.01.2013
 */

#ifndef YLIST_H_
#define YLIST_H_

// Basic linked list
template< typename T >
struct listElement
{
	T data;
	listElement* next;
	listElement( T d, listElement* l ) : data(d), next(l) {};
};


#define ITER_LIST( type, list )		\
	for( listElement< type >* it = list.head; it != 0; it = it->next )
#define ITER_LISTP( type, list )		\
	for( listElement< type >* it = list->head; it != 0; it = it->next )


/*
 * List implements simple linked list container.
 * One way iteration. No support for random access.
 * Last element access. Precalculated count.
 * Sort (recursive merge), find and comparison implemented.
 */


template< typename T >
struct list
{
	listElement<T>* head;
	listElement<T>* tail;
	unsigned int count;

	list( ) : head(0), tail(0), count(0) {};
	list( const list<T>* src ) : head(0), tail(0), count(0) {
		listElement<T>* t = src->head;
		while( t != 0 ){
			push_back(t->data);
			t = t->next;
		}
	}
	~list( ){
		listElement<T>* t = 0;
		while( head != 0 ){
			t = head;
			head = head->next;
			delete t;
		}
	};

	void init( ){
		head = tail = 0;
		count = 0;
	}

	///////////////////
	// List management

	// Insertion of data after element
	void insert( T data, listElement<T>* prev ){
		if( prev == 0 ){
			head = new listElement<T>( data, head );
			if( !head->next )
				tail = head;
		}else{
			prev->next = new listElement<T>( data, prev->next );
			if( prev->next->next == 0 )
				tail = prev->next;
		}
		count++;
	}

	// Pushing of data on the list head
	void push( T data ){
		insert( data, 0 );
	}

	// Pushing of data after the last element
	inline void push_back( T data ){
		insert( data, tail );
	}

	// Removes data at front and returns it
	T pop( )
	{
		T t = head->data;
		remove( head, 0 );
		return t;
	}

	// Remove list content (internal in most cases, you don't need to use this)
	void remove( listElement<T>* t, listElement<T>* prev ){
		if(!t)
			return;
		if( t == head ) // element is head
			head = t->next;
		else
			prev->next = t->next;
		if( tail == t )
			tail = prev;
		delete t;
		t = 0;
		count--;
	}

	// Remove list element by element pointer
	// Returns pointer to next element after removed
	listElement<T>* remove( listElement<T>* element ){
		listElement<T>* t = head;
		listElement<T>* prev = 0;
		if( !element )
			return 0;
		while( t != 0 ){
			if( t == element ){
				remove( t, prev );
				break;
			}
			prev = t;
			t = t->next;
		}
		return prev ? prev->next : head;
	}

	// Remove list element with certain content
	void remove( T data ){
		listElement<T>* t = head;
		listElement<T>* prev = 0;
		while( t != 0 ){
			if( t->data == data )
				break;
			prev = t;
			t = t->next;
		}
		remove( t, prev );
	}

	// Remove all list contents
	void clear( ){
		listElement<T>* t = head;
		if( !t )
			return;
		while( t->next != 0 ){
			remove( t->next, t );
		}
		remove( t, 0 );
		tail = head = 0;
	}


	///////////////////
	// Different list methods

	// Comparison
	bool cmp( const list<T>* src ){
		listElement<T>* self = head;
		listElement<T>* target = src->head;
		while( self != 0 ){
			if( target == 0 || self->data != target->data )
				return false;
			self = self->next;
			target = target->next;
		}
		// If target is not 0 then it has more elements
		if( target == 0 )
			return true;
		return false;
	}

	// Search
	listElement<T>* find( T data ){
		listElement<T>* t = head;
		while( t != 0 ){
			if( t->data == data )
				return t;
			t = t->next;
		}
		return 0;
	}

	// Sorting of list
	template <typename F>
	void sort( F func ){
		if( !head )
			return;
		head = sort_recursive( head, func );
		// FIXME: move it to sort function
		listElement< T >* last = tail;
		while( last->next )
			last = last->next;
		tail = last;
	}

	// Sorting of list part
	template <typename F>
	listElement<T>* sort_recursive(listElement<T>* list_element, F func )
	{
		// Trivial case.
		if (!list_element || !list_element->next)
			return list_element;

		listElement<T>	*right = list_element,
						*temp  = list_element,
						*last  = list_element,
						*result = 0,
						*next   = 0,
						*list_tail = 0;

		// Find halfway through the list (by running two pointers, one at twice the speed of the other).
		while( temp && temp->next ){
			last = right;
			right = right->next;
			temp = temp->next->next;
		}

		// Break the list in two.
		last->next = 0;

		// Recurse on the two smaller lists:
		list_element = sort_recursive( list_element, func );
		right = sort_recursive( right, func );

		// Merge:
		while( list_element || right ){
			// Take from empty lists, or compare:
			if( !right ){
				next = list_element;
				list_element = list_element->next;
			}else if( !list_element ){
				next = right;
				right = right->next;
			}else if( func( list_element->data, right->data ) ){
				next = list_element;
				list_element = list_element->next;
			}else{
				next = right;
				right = right->next;
			}
			if( !result ){
				result=next;
			}else{
				list_tail->next = next;
			}
			list_tail = next;
		}
		return result;
	}

};

#endif /* YLIST_H_ */
