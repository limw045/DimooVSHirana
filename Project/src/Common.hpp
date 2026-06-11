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
const float BOX_WIDTH = 17.941f;
const float BOX_HEIGHT = 5.0f;
const float BOX_DEPTH = 6.0f; // 3D 盒子的前后深度

// 游戏状态
enum GameState {
    STATE_TITLE,
    STATE_ENTRY_ANIMATION,
    STATE_BATTLE,
    STATE_VICTORY_ANIMATION,
    STATE_RESULT
};

// 角色动画状态
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

// 输入缓冲
struct InputState {
    // Player 1 (小野)
    bool hirono_left;
    bool hirono_right;
    bool hirono_up; // 跳跃
    bool hirono_upQueued;
    bool hirono_attack;
    bool hirono_skill;
    bool hirono_ult;
    bool hirono_attackQueued;
    bool hirono_skillQueued;
    bool hirono_ultQueued;

    // Player 2 (Dimoo)
    bool dimoo_left;
    bool dimoo_right;
    bool dimoo_up;  // 跳跃
    bool dimoo_upQueued;
    bool dimoo_attack;
    bool dimoo_skill;
    bool dimoo_ult;
    bool dimoo_attackQueued;
    bool dimoo_skillQueued;
    bool dimoo_ultQueued;

    InputState() {
        hirono_left = hirono_right = hirono_up = hirono_upQueued = false;
        hirono_attack = hirono_skill = hirono_ult = false;
        hirono_attackQueued = hirono_skillQueued = hirono_ultQueued = false;
        dimoo_left = dimoo_right = dimoo_up = dimoo_upQueued = false;
        dimoo_attack = dimoo_skill = dimoo_ult = false;
        dimoo_attackQueued = dimoo_skillQueued = dimoo_ultQueued = false;
    }
};

// 2D 碰撞体定义（X-Y 轴圆圈，格斗碰撞判定）
struct Collider {
    float x, y;
    float radius;

    Collider() : x(0), y(0), radius(0.4f) {}
    Collider(float r) : x(0), y(0), radius(r) {}
};

inline float clamp(float val, float minVal, float maxVal) {
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
}

#endif // COMMON_HPP
