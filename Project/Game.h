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

    // 临时角色坐标（第一阶段用作占位符）
    float hironoX, hironoY, hironoZ;
    float dimooX, dimooY, dimooZ;

private:
    void updateBattle(float dt);
    void drawDebugOverlay();
    void drawHUD();
};

#endif // GAME_HPP
