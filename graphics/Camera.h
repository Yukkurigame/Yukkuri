#ifndef YCAMERA_H
#define YCAMERA_H

#include "basic_types.h"

class Unit;


enum ctMode {
	ctmNormal = 0,	// TARGET_MODE_NORMAL
	ctmCenter 		// TARGET_MODE_CENTER
};


namespace Camera {
	void init( s3f* view );

	void Update();
	float GetX( );
	float GetY( );
	void Move( float X, float Y );
	void ChangeMode( enum ctMode mode );
	void SetTarget( Unit* u );
	void SetTarget( double* X, double* Y );
	Unit* GetTarget( );
	void DeleteTarget( );

};

#endif //YCAMERA_H
