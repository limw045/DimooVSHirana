#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "../Common.hpp"

/**
 * @class Camera
 * @brief Handles 2.5D player tracking and 3D floating orbit camera modes.
 */
class Camera {
public:
    /**
     * @brief Construct a new Camera object with default properties.
     */
    Camera();

    /**
     * @brief Destroy the Camera object.
     */
    ~Camera();

    /**
     * @brief Reset the camera to its default starting position and angles.
     */
    void init();
    
    /**
     * @brief Update the camera position and look-at target.
     * @param dt High-resolution delta time step in seconds.
     * @param char1X Player 1's X coordinate.
     * @param char1Y Player 1's Y coordinate.
     * @param char2X Player 2's X coordinate.
     * @param char2Y Player 2's Y coordinate.
     * @param isUltActive True if a character's ultimate skill is currently performing.
     * @details Computes smooth transitions (lerp) to the target coordinates.
     */
    void update(float dt, float char1X, float char1Y, float char2X, float char2Y, bool isUltActive);
    
    /**
     * @brief Apply view projection matrix transformations via gluLookAt.
     */
    void applyMatrix();

    /**
     * @brief Apply a vibration impulse to the camera.
     * @param intensity Impact vibration magnitude.
     */
    void applyShake(float intensity);

    // Camera positional state (smoothly lerps to targets)
    float x;                ///< Current camera X position
    float y;                ///< Current camera Y position
    float z;                ///< Current camera Z position
    float lookAtX;          ///< Current camera look-at X target
    float lookAtY;          ///< Current camera look-at Y target
    float lookAtZ;          ///< Current camera look-at Z target

    // Positional interpolation targets
    float targetX;
    float targetY;
    float targetZ;
    float targetLookAtX;
    float targetLookAtY;
    float targetLookAtZ;

    // Vibration/shake offsets
    float shakeX;
    float shakeY;
    float shakeZ;
    float shakeTime;
    float shakeIntensity;
    float lerpSpeed;        ///< Smoothing interpolation factor

    // Orbit/Free Camera properties
    bool freeCam;           ///< Toggle for floating camera mode
    float yaw;              ///< Horizontal orbit angle in degrees
    float pitch;            ///< Vertical orbit angle in degrees
    float radius;           ///< Camera distance to center of orbit
    float centerX;          ///< Center look-at X target in free mode
    float centerY;          ///< Center look-at Y target in free mode
    float centerZ;          ///< Center look-at Z target in free mode
};

#endif // CAMERA_HPP
