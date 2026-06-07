#ifndef ARENA_HPP
#define ARENA_HPP

#include "../Common.hpp"

// 盒盖物理状态结构体
struct LidPhysics {
    float angle;
    float velocity;
    float restAngle;
    float damping;
    float springK;

    LidPhysics() : angle(115.0f), velocity(0.0f), restAngle(115.0f), damping(4.0f), springK(25.0f) {}
};

// 气泡纸单元状态 (在 Z=0 附近排成一排作为2D踏板)
struct BubbleCell {
    float x, y;
    float pressDepth;
    float pressVelocity;
    bool popped;

    BubbleCell() : x(0), y(0.005f), pressDepth(0), pressVelocity(0), popped(false) {}
};

// 2D 悬浮/飘落产品说明书
struct Pamphlet {
    float x, y;
    float vx, vy;
    float rot;            // 2D 旋转角
    float rotV;
    bool isFlying;
    float foldAngle;      // 0=对折，1=展开

    Pamphlet() : x(1.0f), y(0.2f), vx(0), vy(0), rot(0), rotV(0), isFlying(false), foldAngle(0.0f) {}
};

// 2D 滑行干燥剂
struct Desiccant {
    float x, y;           // 2D 坐标
    float vx;
    bool popped;

    Desiccant() : x(1.8f), y(0.12f), vx(0), popped(false) {}
};

class Arena {
public:
    Arena();
    ~Arena();

    void init();
    void update(float dt);
    
    // 渲染方法
    void drawOpaque();       // 绘制 3D 盒体 + 2D 纸片道具不透明部分
    void drawTransparent();  // 绘制半透明层

    // 交互触发
    void triggerWallShake(float intensity, bool leftWall);
    void triggerLidShake(float intensity);
    void testExplodeProps();

    // 碰撞检测与响应
    void resolveCollisionWithProps(float charX, float charY, float radius, float& outPushX);

    // 场景物理参数
    LidPhysics lid;
    std::vector<BubbleCell> bubbles;
    Pamphlet pamphlet;
    Desiccant desiccant;

    // 盒壁震动
    float leftWallOffset;
    float rightWallOffset;
    float leftWallShakeTime;
    float rightWallShakeTime;

    // 纹理ID
    unsigned int cardboardTex;
    unsigned int woodTex;
    unsigned int roseLabelTex;
    unsigned int starrySkyTex;
    unsigned int hironoFaceTex;
    unsigned int dimooFaceTex;

private:
    void drawBox();
    void drawLid();
    void drawBubbleWrap();
    void drawPamphlet();
    void drawDesiccant();
    void drawTexturedQuad(float x1, float y1, float z1,
                          float x2, float y2, float z2,
                          float x3, float y3, float z3,
                          float x4, float y4, float z4,
                          float uMax, float vMax);
};

#endif // ARENA_HPP
