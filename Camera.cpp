#include "Camera.h"

YCamera YCamera::CameraControl;

YCamera::YCamera() {
    X = Y = 0.0f;
    TargetX = TargetY = NULL;
    TargetMode = TARGET_MODE_CENTER;
}

void YCamera::OnMove(float MoveX, float MoveY) {
    X += MoveX;
    Y += MoveY;
}

float YCamera::GetX() {
    if(TargetX != NULL) {
        if(TargetMode == TARGET_MODE_CENTER) {
            return *TargetX - (WWIDTH / 2);
        }
        return *TargetX;
    }
    return X;
}

float YCamera::GetY() {
    if(TargetY != NULL) {
        if(TargetMode == TARGET_MODE_CENTER) {
            return *TargetY - (WHEIGHT / 2);
        }
        return *TargetY;
    }
    return Y;
}

void YCamera::SetPos(float X, float Y) {
    this->X = X;
    this->Y = Y;
}

void YCamera::SetTarget(float* X, float* Y) {
    TargetX = X;
    TargetY = Y;
}
