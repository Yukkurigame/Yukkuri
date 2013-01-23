/*
 * basic_types.h
 *
 *  Created on: 11.07.2012
 *
 */

#ifndef BASIC_TYPES_H_
#define BASIC_TYPES_H_

#ifndef MAX
	#define MAX( a, b ) ( ((a) > (b)) ? (a) : (b) )
#endif

#ifndef LOWER_LIMIT
	#define LOWER_LIMIT( val, limit,  _default )	\
	if( val < limit ) val = _default;
#endif

// TODO: find all same structures and group in one.
enum type_identifier {
	tiNone = 0, tiInt, tiFloat, tiString,
};

enum GameCoreBindPoints {
	gdbFIRST = 0, gbpFPS, gbpLAST
};


struct s2i
{
	signed int x;
	signed int y;
	s2i() : x(), y() {}
	s2i( signed int X, signed int Y ) : x(X), y(Y) {}
};


struct s2f
{
	float x;
	float y;
	s2f() : x(), y() {}
	s2f( float X, float Y ) : x(X), y(Y) {}
	s2f operator*( const s2f& b ){
		return s2f( x * b.x, y * b.y );
	}
};


struct s3f
{
	float x;
	float y;
	float z;
	s3f() : x(), y(), z() {}
	s3f( s3f* c ) : x(c->x), y(c->y), z(c->z) {}
	s3f( float X, float Y, float Z ) : x(X), y(Y), z(Z) {}
	void operator-=( const s3f& b ){
		x -= b.x; y -= b.y; z -= b.z;
	}
	void operator+=( const s3f& b ){
		x += b.x; y += b.y; z += b.z;
	}
	s3f operator-( const s3f& b ){
		return s3f( x - b.x, y - b.y, z - b.z );
	}
	s3f operator+( const s3f& b ){
		return s3f( x + b.x, y + b.y, z + b.z );
	}
};


struct rect2f
{
	float x;
	float y;
	float width;
	float height;
	rect2f () : x(), y(), width(), height() {}
	rect2f ( float X, float Y, float W, float H ) : x(X), y(Y), width(W), height(H) {}
	rect2f ( const rect2f* src ) : x(src->x), y(src->y), width(src->width), height(src->height) {}
	s2f size() { return s2f( width, height ); }
};


struct rect2i
{
	int x;
	int y;
	int width;
	int height;
	rect2i () : x(), y(), width(), height() {}
	rect2i ( int X, int Y, int W, int H ) : x(X), y(Y), width(W), height(H) {}
	inline bool operator < ( const rect2i& r ) {
		return  r.width * r.height < width * height;
	}
};


// Basic linked list
template< typename T >
struct listElement
{
	T data;
	listElement* next;
	listElement( T d, listElement* l ) : data(d), next(l) {};
};


template< typename T >
struct list
{
	listElement<T>* head;
	listElement<T>* tail;
	list( ) : head(0), tail(0) {};
	list( const list<T>* src ){
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
	void insert( T data, listElement<T>* prev ){
		if( prev == 0 ){
			push( data );
		}else{
			prev->next = new listElement<T>( data, prev->next );
			if( prev->next->next == 0 )
				tail = prev->next;
		}
	}
	void push( T data ){
		head = new listElement<T>( data, head );
		if( !head->next )
			tail = head;
	}
	inline void push_back( T data )
	{
		insert( tail, data );
	}
	void clear( ){
		listElement<T>* t = head;
		if( !t )
			return;
		while( t->next != 0 ){
			remove( t->next, t );
		}
		remove( t, 0 );
	}
	void remove( listElement<T>* t, listElement<T>* prev ){
		if(!t)
			return;
		if( !prev ) // element is head
			head = t->next;
		else
			prev->next = t->next;
		if( tail == t )
			tail = prev;
		delete t;
		t = 0;
	}
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
	int count( ){
		int c = 0;
		listElement<T>* t = head;
		while( t != 0 ){
			++c;
			t = t->next;
		}
		return c;
	}
	listElement<T>* find( T data ){
		listElement<T>* t = head;
		while( t != 0 ){
			if( t->data == data )
				return t;
			t = t->next;
		}
		return 0;
	}
};



#endif /* BASIC_TYPES_H_ */
