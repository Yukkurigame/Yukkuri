#ifndef YCAMERA_H
#define YCAMERA_H

#include "basic_types.h"

class Unit;
class Brush;

enum ctMode {
	ctmNormal = 0,	// TARGET_MODE_NORMAL
	ctmCenter 		// TARGET_MODE_CENTER
};


namespace Camera {

	void init( );

	void push_state( const rect2f* view, const s2f* z );
	void push_state( const rect2f* view );
	void push_state( );
	void pop_state( );
	int states_count( );

	float* inversed_rotation();

	void update( );
	s3f position( );
	float getX( );
	float getY( );
	void Move( float X, float Y, float Z = 0 );
	void Translate( float X, float Y, float Z = 0 );
	void Rotate( float angle, float X, float Y, float Z );
	void ChangeMode( enum ctMode mode );
	void SetTarget( Unit* u );
	void SetTarget( double* X, double* Y );
	Unit* GetTarget( );
	void DeleteTarget( );

};

#endif //YCAMERA_H
