/*
 * basic_types.h
 *
 *  Created on: 11.07.2012
 *
 */

#ifndef BASIC_TYPES_H_
#define BASIC_TYPES_H_

// TODO: find all same structures and group in one.
enum type_identifier {
	tiNone = 0, tiInt, tiFloat, tiString,
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
};


struct s3f
{
	float x;
	float y;
	float z;
	s3f() : x(), y(), z() {}
	s3f( s3f* c ) : x(c->x), y(c->y), z(c->z) {}
	s3f( float X, float Y, float Z ) : x(X), y(Y), z(Z) {}
	void operator-=(const s3f& b){
		x -= b.x; y -= b.y; z -= b.z;
	}
	s3f operator-(const s3f& b){
		s3f result = *this;
		result.x -= b.x;
		result.y -= b.y;
		result.z -= b.z;
		return result;
	}
	s3f operator+(const s3f& b){
		s3f result = *this;
		result.x += b.x;
		result.y += b.y;
		result.z += b.z;
		return result;
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
	list( ) : head(0) {};
	~list( ){
		listElement<T>* t = 0;
		while( head != 0 ){
			t = head;
			head = head->next;
			delete t;
		}
	};
	void insert( T data, listElement<T>* prev ){
		if( prev == 0 )
			push( data );
		else
			prev->next = new listElement<T>( data, prev->next );
	}
	void push( T data ){
		head = new listElement<T>( data, head );
	}
	void remove( listElement<T>* t, listElement<T>* prev ){
		if(!t)
			return;
		if( !prev ) // element is head
			head = t->next;
		else
			prev->next = t->next;
		delete t;
		t = 0;
	}
	void remove( listElement<T>* element ){
		listElement<T>* t = head;
		listElement<T>* prev = 0;
		if( !element )
			return;
		while( t != 0 ){
			if( t == element ){
				remove( t, prev );
				break;
			}
			prev = t;
			t = t->next;
		}
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
};



#endif /* BASIC_TYPES_H_ */
