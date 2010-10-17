#ifndef YCAMERA_H
#define YCAMERA_H

#include "Define.h"
#include <cstdlib>

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
        void SetTarget( float* X, float* Y );

    private:
        int TargetMode;
        float posX;
        float posY;
        float offsetX;
        float offsetY;
        float* TargetX;
        float* TargetY;
};

#endif //YCAMERA_H
