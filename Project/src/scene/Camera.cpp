#include "Camera.h"
#include <cstdlib>
#include <iostream>

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

    // Free camera defaults
    freeCam = false;
    yaw = 0.0f;
    pitch = 15.0f;
    radius = 5.5f;
    centerX = 0.0f;
    centerY = 2.5f; // Middle of box height
    centerZ = 0.0f;
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

    freeCam = false;
    yaw = 0.0f;
    pitch = 15.0f;
    radius = 5.5f;
    centerX = 0.0f;
    centerY = 2.5f;
    centerZ = 0.0f;
}

void Camera::update(float dt, float char1X, float char1Y, float char2X, float char2Y, bool isUltActive) {
    if (dt < 0.0f) {
        return;
    }
    
    // Clamp delta time to avoid coordinate jumping on lag spikes
    dt = clamp(dt, 0.0f, 0.05f);

    if (freeCam) {
        // Clamp camera orbit bounds to prevent gimbal inversion
        pitch = clamp(pitch, -85.0f, 85.0f);
        radius = clamp(radius, 1.5f, 15.0f);

        // Convert spherical orbit coordinates to Cartesian coordinates
        float pitchRad = pitch * M_PI / 180.0f;
        float yawRad = yaw * M_PI / 180.0f;

        targetX = centerX + radius * cos(pitchRad) * sin(yawRad);
        targetY = centerY + radius * sin(pitchRad);
        targetZ = centerZ + radius * cos(pitchRad) * cos(yawRad);

        targetLookAtX = centerX;
        targetLookAtY = centerY;
        targetLookAtZ = centerZ;
    } else {
        // standard automatic 2.5D tracking mode
        float midX = (char1X + char2X) / 2.0f;
        float midY = (char1Y + char2Y) / 2.0f;
        
        float dx = char1X - char2X;
        float dy = char1Y - char2Y;
        float dist = sqrt(dx*dx + dy*dy);

        if (isUltActive) {
            targetX = midX;
            targetY = midY + 0.5f;
            targetZ = 1.8f;
            targetLookAtX = midX;
            targetLookAtY = midY + 0.4f;
            targetLookAtZ = 0.0f;
        } else {
            targetX = midX; 
            // Rise camera height dynamically as characters move apart
            targetY = 1.2f + dist * 0.22f;
            // Pull back camera distance dynamically to keep characters in frame with a safety margin
            targetZ = 3.0f + dist * 0.72f;
            
            targetLookAtX = midX;
            // Keep the look-at target relatively low to force a downward pitch angle
            targetLookAtY = 1.0f + dist * 0.05f;
            targetLookAtZ = 0.0f;
        }

        // Limit the automatic camera tracking boundary so it doesn't clip out of the box
        // Adjusted for the wide arena and extended depth range to prevent character out-of-frame and floor clipping
        targetX = clamp(targetX, -BOX_WIDTH / 2.0f + 2.5f, BOX_WIDTH / 2.0f - 2.5f);
        targetZ = clamp(targetZ, 3.5f, 16.0f);
    }

    // Smooth camera transition interpolate (Lerp)
    x += (targetX - x) * lerpSpeed * dt;
    y += (targetY - y) * lerpSpeed * dt;
    z += (targetZ - z) * lerpSpeed * dt;

    lookAtX += (targetLookAtX - lookAtX) * lerpSpeed * dt;
    lookAtY += (targetLookAtY - lookAtY) * lerpSpeed * dt;
    lookAtZ += (targetLookAtZ - lookAtZ) * lerpSpeed * dt;

    // Camera screen shake decay
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
    shakeTime = 0.15f;
    shakeIntensity = intensity;
}
