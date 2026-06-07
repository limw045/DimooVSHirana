#include "Camera.h"
#include <cstdlib>

Camera::Camera() {
    x = targetX = 0.0f;
    y = targetY = 5.0f;
    z = targetZ = 7.0f;

    lookAtX = targetLookAtX = 0.0f;
    lookAtY = targetLookAtY = 1.0f;
    lookAtZ = targetLookAtZ = 0.0f;

    shakeX = shakeY = shakeZ = 0.0f;
    shakeTime = 0.0f;
    shakeIntensity = 0.0f;
    lerpSpeed = 3.0f;
}

Camera::~Camera() {}

void Camera::init() {
    x = 0.0f;
    y = 5.0f;
    z = 7.0f;
    lookAtX = 0.0f;
    lookAtY = 1.0f;
    lookAtZ = 0.0f;
    shakeX = shakeY = shakeZ = 0.0f;
    shakeTime = 0.0f;
}

void Camera::update(float dt, float char1X, float char1Z, float char2X, float char2Z, bool isUltActive) {
    // 1. 计算两个角色的中心点和距离
    float midX = (char1X + char2X) / 2.0f;
    float midZ = (char1Z + char2Z) / 2.0f;
    
    float dx = char1X - char2X;
    float dz = char1Z - char2Z;
    float dist = sqrt(dx*dx + dz*dz);

    // 2. 根据对战距离动态决定相机的高宽比例拉伸
    if (isUltActive) {
        // 大招演出特写镜头：拉近、降低高度
        targetX = midX;
        targetY = 2.2f;
        targetZ = 4.0f;
        targetLookAtX = midX;
        targetLookAtY = 1.0f;
        targetLookAtZ = midZ;
    } else {
        // 正常对战跟随镜头
        targetX = midX * 0.8f; // 平滑偏置
        targetY = 4.0f + dist * 0.35f; // 距离越远拉得越高
        targetZ = 6.0f + dist * 0.55f; // 距离越远拉得越后
        
        targetLookAtX = midX;
        targetLookAtY = 1.0f;
        targetLookAtZ = midZ * 0.5f;
    }

    // 限位，不让相机飞出竞技场范围过远
    targetX = clamp(targetX, -BOX_WIDTH / 2.0f + 1.0f, BOX_WIDTH / 2.0f - 1.0f);
    targetZ = clamp(targetZ, 3.5f, 15.0f);

    // 3. 平滑插值 (Lerp)
    x += (targetX - x) * lerpSpeed * dt;
    y += (targetY - y) * lerpSpeed * dt;
    z += (targetZ - z) * lerpSpeed * dt;

    lookAtX += (targetLookAtX - lookAtX) * lerpSpeed * dt;
    lookAtY += (targetLookAtY - lookAtY) * lerpSpeed * dt;
    lookAtZ += (targetLookAtZ - lookAtZ) * lerpSpeed * dt;

    // 4. 处理镜头震动衰减
    if (shakeTime > 0.0f) {
        shakeTime -= dt;
        float factor = shakeTime / 0.15f; // 震动占比因子（0.15秒内衰减到0）
        
        shakeX = ((rand() % 200) / 100.0f - 1.0f) * shakeIntensity * factor;
        shakeY = ((rand() % 200) / 100.0f - 1.0f) * shakeIntensity * 0.6f * factor;
        shakeZ = ((rand() % 200) / 100.0f - 1.0f) * shakeIntensity * 0.4f * factor;
        
        if (shakeTime <= 0.0f) {
            shakeX = shakeY = shakeZ = 0.0f;
        }
    }
}

void Camera::applyMatrix() {
    gluLookAt(x + shakeX, y + shakeY, z + shakeZ,
              lookAtX, lookAtY, lookAtZ,
              0.0f, 1.0f, 0.0f);
}

void Camera::applyShake(float intensity) {
    shakeTime = 0.15f;        // 震动持续 0.15 秒
    shakeIntensity = intensity;
}
