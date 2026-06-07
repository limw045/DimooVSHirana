#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "../Common.hpp"

class Camera {
public:
    Camera();
    ~Camera();

    void init();
    
    // 根据角色位置动态更新相机位置（XZ平面中点及距离缩放）
    void update(float dt, float char1X, float char1Z, float char2X, float char2Z, bool isUltActive);
    
    // 应用视图变换矩阵 (调用 gluLookAt)
    void applyMatrix();

    // 触发相机震动
    void applyShake(float intensity);

    // 相机当前坐标与看点
    float x, y, z;
    float lookAtX, lookAtY, lookAtZ;

    // 目标坐标与看点
    float targetX, targetY, targetZ;
    float targetLookAtX, targetLookAtY, targetLookAtZ;

    // 震动偏移
    float shakeX, shakeY, shakeZ;
    float shakeTime;
    float shakeIntensity;
    float lerpSpeed; // 平滑运动插值速度
};

#endif // CAMERA_HPP
