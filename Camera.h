#ifndef YCAMERA_H
#define YCAMERA_H

#include "Unit.h"

enum {
	TARGET_MODE_NORMAL = 0,
	TARGET_MODE_CENTER
};

class YCamera {
	public:
		YCamera();

		static YCamera CameraControl;

		void Update();

		float GetX( );
		float GetY( );
		void Move( float X, float Y );
		void ChangeMode( int mode );
		void SetTarget( Unit* u );
		void SetTarget( float* X, float* Y );
		void DeleteTarget( );

	private:
		int TargetMode;
		float posX;
		float posY;
		float offsetX;
		float offsetY;
		float* TargetX;
		float* TargetY;
		Unit* Target;
};

#endif //YCAMERA_H
