#include "Camera.h"
#include <cstdlib>

Camera::Camera() {
    x = targetX = 0.0f;
    y = targetY = 1.3f;
    z = targetZ = 4.5f;

    lookAtX = targetLookAtX = 0.0f;
    lookAtY = targetLookAtY = 0.9f;
    lookAtZ = targetLookAtZ = 0.0f;

    shakeX = shakeY = shakeZ = 0.0f;
    shakeTime = 0.0f;
    shakeIntensity = 0.0f;
    lerpSpeed = 3.5f;
}

Camera::~Camera() {}

void Camera::init() {
    x = 0.0f;
    y = 1.3f;
    z = 4.5f;
    lookAtX = 0.0f;
    lookAtY = 0.9f;
    lookAtZ = 0.0f;
    shakeX = shakeY = shakeZ = 0.0f;
    shakeTime = 0.0f;
}

void Camera::update(float dt, float char1X, float char1Y, float char2X, float char2Y, bool isUltActive) {
    // 1. 计算两个角色的中心点和距离
    float midX = (char1X + char2X) / 2.0f;
    float midY = (char1Y + char2Y) / 2.0f;
    
    float dx = char1X - char2X;
    float dy = char1Y - char2Y;
    float dist = sqrt(dx*dx + dy*dy);

    // 2. 经典横版格斗游戏视角（平视微俯视，低机位，焦距随距离平滑缩放）
    if (isUltActive) {
        // 大招特写镜头
        targetX = midX;
        targetY = midY + 0.5f;
        targetZ = 1.8f;
        targetLookAtX = midX;
        targetLookAtY = midY + 0.4f;
        targetLookAtZ = 0.0f;
    } else {
        // 正常格斗侧视：摄像机水平居中跟踪，低高度
        targetX = midX; 
        targetY = midY + 0.6f;          // 稍微偏高一点以看到纸箱内部底部
        targetZ = 2.8f + dist * 0.45f;  // 动态焦距（保证两个角色始终同屏且清晰可见）
        
        targetLookAtX = midX;
        targetLookAtY = midY + 0.5f;    // 锁定视线在角色中部高度
        targetLookAtZ = 0.0f;           // 锁定在 Z = 0 的格斗平面
    }

    // 限制相机边界，防止飞出纸盒外面
    targetX = clamp(targetX, -BOX_WIDTH / 2.0f + 1.0f, BOX_WIDTH / 2.0f - 1.0f);
    targetZ = clamp(targetZ, 2.0f, 6.0f);

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
        float factor = shakeTime / 0.15f;
        
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
