#ifndef YCAMERA_H
#define YCAMERA_H

#include "basic_types.h"

class Unit;


enum ctMode {
	ctmNormal = 0,	// TARGET_MODE_NORMAL
	ctmCenter 		// TARGET_MODE_CENTER
};


namespace Camera {

	void push_state( const rect2f* view );
	void pop_state( );

	const float* mvp( );
	const float* offset( );

	void update( );
	s3f position( );
	float getX( );
	float getY( );
	void Translate( float X, float Y, float Z = 0 );
	void Rotate( float angle, float X, float Y, float Z );
	void ChangeMode( enum ctMode mode );
	void SetTarget( Unit* u );
	void SetTarget( double* X, double* Y );
	Unit* GetTarget( );
	void DeleteTarget( );

};

#endif //YCAMERA_H
