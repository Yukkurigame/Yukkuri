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
        
        int TargetMode;
        float GetX();
        float GetY();
        void OnMove(float MoveX, float MoveY);
        void SetPos(float X, float Y);
        void SetTarget(float* X, float* Y);

    private:
        float X;
        float Y;
        float* TargetX;
        float* TargetY;
};

#endif //YCAMERA_H
