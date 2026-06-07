#ifndef GAME_HPP
#define GAME_HPP

#include "Common.hpp"
#include "scene/Arena.h"
#include "scene/Camera.h"
#include "scene/Lighting.h"

class Game {
public:
    Game();
    ~Game();

    void init();
    void update(float dt);
    void draw();

    // 键盘按键与松开回调
    void handleInput(unsigned char key);
    void handleSpecialInput(int key);


    // 属性
    GameState currentState;
    float stateTimer;
    InputState input;

    // 场景核心组件
    Arena arena;
    Camera camera;
    Lighting lighting;

    // 调试模式设置
    bool debugMode;
    bool showColliders;
    bool isDummyAI;

    // 临时角色坐标与速度（2.5D版）
    float hironoX, hironoY, hironoZ;
    float hironoVy;
    bool hironoFacingRight;

    float dimooX, dimooY, dimooZ;
    float dimooVy;
    bool dimooFacingRight;

    // 战斗与回合属性
    float hironoHp, hironoMaxHp;
    float dimooHp, dimooMaxHp;
    float matchTimer;
    bool matchOver;

    // 粒子系统
    struct GameParticle {
        float x, y, z;
        float vx, vy, vz;
        float r, g, b, a;
        float size;
        float life;
        float maxLife;
    };
    std::vector<GameParticle> particles;

    void spawnHitSparks(float x, float y, float z, int count, float r, float g, float b);
    void spawnDust(float x, float y, float z, int count);

private:
    void updateBattle(float dt);
    void drawDebugOverlay();
    void drawHUD();
};

#endif // GAME_HPP
