#ifndef YCAMERA_H
#define YCAMERA_H

#include "Unit.h"

enum ctMode {
	ctmNormal = 0,	// TARGET_MODE_NORMAL
	ctmCenter 		// TARGET_MODE_CENTER
};

namespace Camera {
	void init( );

	void Update();
	float GetX( );
	float GetY( );
	void Move( float X, float Y );
	void ChangeMode( enum ctMode mode );
	void SetTarget( Unit* u );
	void SetTarget( float* X, float* Y );
	Unit* GetTarget( );
	void DeleteTarget( );

};

#endif //YCAMERA_H
