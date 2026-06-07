#ifndef ARENA_HPP
#define ARENA_HPP

#include "../Common.hpp"

// 盒盖物理状态结构体
struct LidPhysics {
    float angle;        // 当前旋转角度（相对于后壁顶边）
    float velocity;     // 旋转角速度
    float restAngle;    // 静止时的角度（默认半开 115 度）
    float damping;      // 阻尼系数
    float springK;      // 弹簧恢复系数

    LidPhysics() : angle(115.0f), velocity(0.0f), restAngle(115.0f), damping(4.0f), springK(25.0f) {}
};

// 气泡纸单元状态
struct BubbleCell {
    float x, z;
    float pressDepth;     // 下压深度（0.0为正常，1.0为完全压扁）
    float pressVelocity;
    bool popped;

    BubbleCell() : x(0), z(0), pressDepth(0), pressVelocity(0), popped(false) {}
};

// 产品说明书折纸状态
struct Pamphlet {
    float x, y, z;
    float vx, vy, vz;
    float rotX, rotY, rotZ;
    float rotVX, rotVY, rotVZ;
    bool isFlying;
    float foldAngle;      // 0.0 为完全折叠，1.0 为展开

    Pamphlet() : x(1.0f), y(0.01f), z(-0.5f), vx(0), vy(0), vz(0),
                 rotX(0), rotY(0), rotZ(0), rotVX(0), rotVY(0), rotVZ(0),
                 isFlying(false), foldAngle(0.0f) {}
};

// 干燥剂包状态
struct Desiccant {
    float x, y, z;
    float vx, vz;
    bool popped;

    Desiccant() : x(2.0f), y(0.05f), z(1.5f), vx(0), vz(0), popped(false) {}
};

class Arena {
public:
    Arena();
    ~Arena();

    void init();
    void update(float dt);
    
    // 渲染方法
    void drawOpaque();       // 绘制不透明底座、盒壁、道具
    void drawTransparent();  // 绘制半透明前壁、地面雾气

    // 交互触发器
    void triggerWallShake(float intensity, bool leftWall);
    void triggerLidShake(float intensity);
    void testExplodeProps(); // 调试用：引爆场景道具

    // 获取碰撞边界
    float getWidth() const { return BOX_WIDTH; }
    float getDepth() const { return BOX_DEPTH; }
    float getHeight() const { return BOX_HEIGHT; }

    // 碰撞检测与响应
    void resolveCollisionWithProps(float charX, float charZ, float radius, float& outPushX, float& outPushZ);

    // 场景物理参数
    LidPhysics lid;
    std::vector<BubbleCell> bubbles;
    Pamphlet pamphlet;
    Desiccant desiccant;

    // 盒壁震动偏移（用于渲染）
    float leftWallOffset;
    float rightWallOffset;
    float leftWallShakeTime;
    float rightWallShakeTime;

    // 纹理ID
    unsigned int cardboardTex;
    unsigned int woodTex;
    unsigned int roseLabelTex;
    unsigned int starrySkyTex;

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
