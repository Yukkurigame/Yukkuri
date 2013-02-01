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
	rect2f ( const rect2f& src ) : x(src.x), y(src.y), width(src.width), height(src.height) {}
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






#endif /* BASIC_TYPES_H_ */
