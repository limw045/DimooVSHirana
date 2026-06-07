#ifndef COMMON_HPP
#define COMMON_HPP

#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 竞技场尺寸常量
const float BOX_WIDTH = 6.0f;
const float BOX_HEIGHT = 5.0f;
const float BOX_DEPTH = 6.0f;

// 游戏状态枚举
enum GameState {
    STATE_TITLE,
    STATE_ENTRY_ANIMATION,
    STATE_BATTLE,
    STATE_VICTORY_ANIMATION,
    STATE_RESULT
};

// 角色动画状态枚举
enum AnimState {
    ANIM_ENTRY,
    ANIM_IDLE,
    ANIM_WALK,
    ANIM_HIT,
    ANIM_SKILL1,
    ANIM_ULT,
    ANIM_VICTORY,
    ANIM_DEFEAT
};

// 输入缓冲结构体
struct InputState {
    // Player 1 (小野)
    bool hirono_left;
    bool hirono_right;
    bool hirono_up;
    bool hirono_down;
    bool hirono_skill1_pressed;
    bool hirono_skill3_pressed;

    // Player 2 (Dimoo)
    bool dimoo_left;
    bool dimoo_right;
    bool dimoo_up;
    bool dimoo_down;
    bool dimoo_skill1_pressed;
    bool dimoo_skill3_pressed;

    InputState() {
        hirono_left = hirono_right = hirono_up = hirono_down = false;
        hirono_skill1_pressed = hirono_skill3_pressed = false;
        dimoo_left = dimoo_right = dimoo_up = dimoo_down = false;
        dimoo_skill1_pressed = dimoo_skill3_pressed = false;
    }
};

// 碰撞体定义（XZ平面圆形 + Y轴高度）
struct Collider {
    float x, z;
    float radius;
    float yMin, yMax;

    Collider() : x(0), z(0), radius(0.4f), yMin(0), yMax(1.2f) {}
    Collider(float r, float ymin, float ymax) : x(0), z(0), radius(r), yMin(ymin), yMax(ymax) {}
};

// 简易数学辅助函数
inline float clamp(float val, float minVal, float maxVal) {
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
}

#endif // COMMON_HPP
