#ifndef ARENA_HPP
#define ARENA_HPP

#include "../Common.hpp"

/**
 * @struct LidPhysics
 * @brief Represents the physical simulation parameters of the box lid hinge.
 */
struct LidPhysics {
    float angle;        ///< Current hinge rotation angle in degrees
    float velocity;     ///< Rotational velocity of the lid
    float restAngle;    ///< Relaxed resting angle of the lid (115 degrees)
    float damping;      ///< Rotational friction damping coefficient
    float springK;      ///< Torsional spring stiffness coefficient

    LidPhysics() : angle(115.0f), velocity(0.0f), restAngle(115.0f), damping(4.0f), springK(25.0f) {}
};

/**
 * @struct BubbleCell
 * @brief Represents a single bubble wrap element on the arena floor (kept as a skeleton).
 */
struct BubbleCell {
    float x, y, z;
    float pressDepth;
    float pressVelocity;
    bool popped;

    BubbleCell() : x(0), y(0.005f), z(0), pressDepth(0), pressVelocity(0), popped(false) {}
};

/**
 * @struct Pamphlet
 * @brief Represents the product instructions booklet prop (kept as a skeleton).
 */
struct Pamphlet {
    float x, y;
    float vx, vy;
    float rot;
    float rotV;
    bool isFlying;
    float foldAngle;

    Pamphlet() : x(1.0f), y(0.2f), vx(0), vy(0), rot(0), rotV(0), isFlying(false), foldAngle(0.0f) {}
};

/**
 * @struct Desiccant
 * @brief Represents the desiccant packet prop (kept as a skeleton).
 */
struct Desiccant {
    float x, y;
    float vx;
    bool popped;

    Desiccant() : x(1.8f), y(0.12f), vx(0), popped(false) {}
};

struct LightBeam {
    float sourceX, sourceY, sourceZ;
    float targetX, targetY, targetZ;
    float r, g, b, maxAlpha;
    float sourceRadius, targetRadius;
    float flickerPhase;
    bool active;
};

struct BoxHole {
    float cx, cy, cz;
    float radius;
    float hpThreshold;
    char wall;
    bool active;
    int seed;
};

struct DustParticle {
    float x, y, z;
    float vx, vy, vz;
    float size;
    float alpha;
    float life;
    float maxLife;
};

struct SpaceObject {
    float x, y, z;
    float vx, vy, vz;
    float rx, ry, rz;    // rotation angles
    float rvx, rvy, rvz; // rotation velocities
    float size;
    int seed;
    int type;            // 0: asteroid, 1: Hirono box, 2: Dimoo box, 3: Cardboard box, 4: Rose label box
};


/**
 * @class Arena
 * @brief Manages the 3D cardboard box arena, boundary collisions, textures, and hinge physics.
 */
class Arena {
public:
    /**
     * @brief Construct the Arena object and initialize floor bubble wrappers.
     */
    Arena();

    /**
     * @brief Clean up loaded OpenGL textures.
     */
    ~Arena();

    /**
     * @brief Initialize OpenGL textures and build-in display configurations.
     * @details Loads specified images from f:\Degree\Last Sem\TCG\Project\images.
     */
    void init();

    /**
     * @brief Update physics state for the box lid, wall shakes, and item stubs.
     * @param dt High-resolution delta time step in seconds.
     */
    void update(float dt);
    
    /**
     * @brief Render the opaque parts of the arena (Floor, Back Wall, Side Walls, Flaps, Lid).
     */
    void drawOpaque();

    /**
     * @brief Render semi-transparent elements of the arena.
     */
    void drawTransparent();

    /**
     * @brief Trigger a vibration of the left or right wall.
     * @param intensity Vibration amplitude.
     * @param leftWall True to vibrate left wall, false to vibrate right wall.
     */
    void triggerWallShake(float intensity, bool leftWall);

    /**
     * @brief Apply a physical impulse to the lid hinge.
     * @param intensity Angular impulse magnitude.
     */
    void triggerLidShake(float intensity);

    /**
     * @brief Trigger instructions flyer flight simulation.
     */
    void testExplodeProps();

    /**
     * @brief Perform collision checks between character and floor props.
     * @param charX Character's current X coordinate.
     * @param charY Character's current Y coordinate.
     * @param radius Character's physical bounding circle radius.
     * @param outPushX Output offset vector to push the character out of collision.
     */
    void resolveCollisionWithProps(float charX, float charY, float radius, float& outPushX);

    // HP-Dynamic lighting & Tyndall effects
    float hironoHpPct;
    float dimooHpPct;
    float minHpPct;

    std::vector<BoxHole> holes;
    std::vector<LightBeam> beams;
    std::vector<DustParticle> dustParticles;
    std::vector<SpaceObject> spaceObjects;

    void setHpPercentages(float h1Hp, float h1Max, float h2Hp, float h2Max);
    void initHolesAndBeams();
    void updateLightBeamsAndParticles(float dt);
    void drawLightBeams();
    void drawDustParticles();
    void draw3DTornFlaps(const BoxHole& hole);

    // Dynamic Hinge Lid
    LidPhysics lid;

    // Floor Bubble wrappers (skeleton compatible with Game.cpp)
    std::vector<BubbleCell> bubbles;

    // Folding pamphlet flyer (skeleton compatible with Game.cpp)
    Pamphlet pamphlet;

    // Sliding desiccant bag (skeleton compatible with Game.cpp)
    Desiccant desiccant;

    // Wall vibration offsets
    float leftWallOffset;
    float rightWallOffset;
    float leftWallShakeTime;
    float rightWallShakeTime;

    // Standard Texture IDs required by Game.cpp
    unsigned int cardboardTex;
    unsigned int woodTex;
    unsigned int roseLabelTex;
    unsigned int starrySkyTex;
    unsigned int hironoFaceTex;
    unsigned int dimooFaceTex;

    // Dedicated Arena boundary textures
    unsigned int floorTex;        ///< images/Floor.jpg
    unsigned int leftRightTex;    ///< images/LeftRight.jpg
    unsigned int backTex;         ///< images/Background.jpg

private:
    /**
     * @brief Render the main 3D cardboard box geometry.
     */
    void drawBox();

    /**
     * @brief Render the interactive hinge lid.
     */
    void drawLid();

    /**
     * @brief Draw a textured quad in 3D space with safe coordinate mapping.
     */
    void drawTexturedQuad(float x1, float y1, float z1,
                          float x2, float y2, float z2,
                          float x3, float y3, float z3,
                          float x4, float y4, float z4,
                          float uMax, float vMax);
};

#endif // ARENA_HPP
