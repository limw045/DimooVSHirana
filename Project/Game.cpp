#include "Game.h"
#include <iostream>
#include <sstream>

// 辅助方法：绘制位图字符（UI文字渲染）
static void drawString(void* font, const std::string& str, float x, float y) {
    glRasterPos2f(x, y);
    for (size_t i = 0; i < str.size(); i++) {
        glutBitmapCharacter(font, str[i]);
    }
}

// 辅助方法：绘制线框圆柱体（用于碰撞盒可视化）
static void drawWireCylinder(float r, float h, int segments) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)segments;
        glVertex3f(r * cos(theta), 0.0f, r * sin(theta));
    }
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)segments;
        glVertex3f(r * cos(theta), h, r * sin(theta));
    }
    glEnd();
    
    glBegin(GL_LINES);
    for (int i = 0; i < segments; i += segments/4) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)segments;
        float cx = r * cos(theta);
        float cz = r * sin(theta);
        glVertex3f(cx, 0.0f, cz);
        glVertex3f(cx, h, cz);
    }
    glEnd();
}

Game::Game() {
    currentState = STATE_TITLE;
    stateTimer = 0.0f;
    
    debugMode = true;
    showColliders = false;
    isDummyAI = false;

    // 初始位置设置
    hironoX = -1.5f; hironoY = 0.0f; hironoZ = 0.0f;
    dimooX = 1.5f;   dimooY = 0.1f;  dimooZ = 0.0f; // Dimoo 稍微悬空
}

Game::~Game() {}

void Game::init() {
    currentState = STATE_TITLE;
    stateTimer = 0.0f;
    
    hironoX = -1.5f; hironoY = 0.0f; hironoZ = 0.0f;
    dimooX = 1.5f;   dimooY = 0.1f;  dimooZ = 0.0f;

    arena.init();
    camera.init();
    lighting.init();
}

void Game::update(float dt) {
    stateTimer += dt;

    switch (currentState) {
        case STATE_TITLE:
            // 标题界面：保持相机静止旋转
            camera.update(dt, 0.0f, 0.0f, 0.0f, 0.0f, false);
            camera.x = 0.0f; camera.y = 5.0f; camera.z = 7.5f;
            camera.lookAtX = 0.0f; camera.lookAtY = 1.0f; camera.lookAtZ = 0.0f;
            arena.update(dt);
            break;

        case STATE_ENTRY_ANIMATION:
            // 自动播放出场动画，第一阶段用 2 秒延时跳过
            camera.update(dt, hironoX, hironoZ, dimooX, dimooZ, false);
            arena.update(dt);
            if (stateTimer >= 2.0f) {
                currentState = STATE_BATTLE;
                stateTimer = 0.0f;
                std::cout << "[Game] Entered Battle State!" << std::endl;
            }
            break;

        case STATE_BATTLE:
            updateBattle(dt);
            break;

        case STATE_VICTORY_ANIMATION:
            camera.update(dt, hironoX, hironoZ, dimooX, dimooZ, false);
            arena.update(dt);
            if (stateTimer >= 3.0f) {
                currentState = STATE_RESULT;
                stateTimer = 0.0f;
            }
            break;

        case STATE_RESULT:
            // 结果展示界面，等待重置按键
            camera.update(dt, hironoX, hironoZ, dimooX, dimooZ, false);
            arena.update(dt);
            break;
    }
}

void Game::updateBattle(float dt) {


    // 3. 边界碰撞限制 (防止跑出纸盒底面)
    float borderW = BOX_WIDTH / 2.0f - 0.4f; // 扣除碰撞半径
    float borderD = BOX_DEPTH / 2.0f - 0.4f;

    hironoX = clamp(hironoX, -borderW, borderW);
    hironoZ = clamp(hironoZ, -borderD, borderD);
    dimooX = clamp(dimooX, -borderW, borderW);
    dimooZ = clamp(dimooZ, -borderD, borderD);

    // 4. 解决道具间的碰撞 (踩扁气泡纸，推开干燥剂)
    float pushX = 0, pushZ = 0;
    arena.resolveCollisionWithProps(hironoX, hironoZ, 0.4f, pushX, pushZ);
    hironoX += pushX; hironoZ += pushZ;

    arena.resolveCollisionWithProps(dimooX, dimooZ, 0.4f, pushX, pushZ);
    dimooX += pushX; dimooZ += pushZ;

    // 5. 解决两个角色（占位球体）之间的互相碰撞
    float dx = dimooX - hironoX;
    float dz = dimooZ - hironoZ;
    float dist = sqrt(dx*dx + dz*dz);
    float minDist = 0.8f; // 0.4f + 0.4f (双方碰撞半径)
    if (dist < minDist && dist > 0.001f) {
        float push = (minDist - dist) / 2.0f;
        hironoX -= (dx / dist) * push;
        hironoZ -= (dz / dist) * push;
        dimooX += (dx / dist) * push;
        dimooZ += (dz / dist) * push;
    }

    // 6. 更新子模块物理
    arena.update(dt);
    camera.update(dt, hironoX, hironoZ, dimooX, dimooZ, false);
    
    // 角色自发光点光源跟着走
    lighting.updateCharacterLights(hironoX, hironoY, hironoZ, dimooX, dimooY, dimooZ, false);
}

void Game::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 设置三维摄像机矩阵
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 16.0 / 9.0, 0.1, 100.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    camera.applyMatrix();
    lighting.applyMainLight(); // 光源需要放在视图变换后，以保持定向光方向相对摄像机恒定

    // --- A. 不透明层渲染 ---
    glDepthMask(GL_TRUE);
    arena.drawOpaque();

    // 在 BATTLE 或其他对战状态下，绘制占位球体代替角色
    if (currentState != STATE_TITLE) {
        // Player 1: Hirono Placeholder (绿球)
        glPushMatrix();
        glTranslatef(hironoX, hironoY + 0.4f, hironoZ);
        glColor3f(0.12f, 0.76f, 0.25f);
        glutSolidSphere(0.4, 20, 20);
        glPopMatrix();

        // Player 2: Dimoo Placeholder (蓝球)
        glPushMatrix();
        glTranslatef(dimooX, dimooY + 0.4f, dimooZ);
        glColor3f(0.20f, 0.50f, 0.85f);
        glutSolidSphere(0.4, 20, 20);
        glPopMatrix();
    }

    // --- B. 半透明层渲染 ---
    glDepthMask(GL_FALSE);
    arena.drawTransparent();
    glDepthMask(GL_TRUE);

    // --- C. Debug 碰撞盒绘制 ---
    if (showColliders && currentState != STATE_TITLE) {
        glDisable(GL_LIGHTING);
        glLineWidth(2.0f);

        // Hirono 碰撞体 (绿色线框)
        glPushMatrix();
        glTranslatef(hironoX, hironoY, hironoZ);
        glColor3f(0.0f, 1.0f, 0.0f);
        drawWireCylinder(0.4f, 1.2f, 16);
        glPopMatrix();

        // Dimoo 碰撞体 (蓝色线框)
        glPushMatrix();
        glTranslatef(dimooX, dimooY, dimooZ);
        glColor3f(0.0f, 0.7f, 1.0f);
        drawWireCylinder(0.4f, 1.2f, 16);
        glPopMatrix();

        // 干燥剂碰撞体 (红色线框)
        glPushMatrix();
        glTranslatef(arena.desiccant.x, arena.desiccant.y - 0.05f, arena.desiccant.z);
        glColor3f(1.0f, 0.0f, 0.0f);
        drawWireCylinder(0.2f, 0.2f, 12);
        glPopMatrix();

        glLineWidth(1.0f);
        glEnable(GL_LIGHTING);
    }

    // --- D. 二维 UI 层渲染 ---
    drawHUD();
}

void Game::drawHUD() {
    // 切换到正交投影渲染 2D 文本和状态
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1280, 0, 720);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    if (currentState == STATE_TITLE) {
        // 渲染主标题画面
        glColor4f(0.0f, 0.0f, 0.0f, 0.65f);
        glEnable(GL_BLEND);
        glBegin(GL_QUADS);
        glVertex2f(0, 0); glVertex2f(1280, 0);
        glVertex2f(1280, 720); glVertex2f(0, 720);
        glEnd();
        glDisable(GL_BLEND);

        glColor3f(1.0f, 0.85f, 0.15f);
        drawString(GLUT_BITMAP_TIMES_ROMAN_24, "BLIND BOX BATTLE", 510, 420);
        
        glColor3f(0.9f, 0.9f, 0.9f);
        drawString(GLUT_BITMAP_HELVETICA_18, "Press [SPACE] or [ENTER] to open blind boxes", 440, 320);
        
        glColor3f(0.6f, 0.6f, 0.6f);
        drawString(GLUT_BITMAP_HELVETICA_12, "Player 1 (Hirono): WASD | Player 2 (Dimoo): Arrow Keys", 490, 240);
        drawString(GLUT_BITMAP_HELVETICA_12, "Debug Keys: F4 (Colliders) | F5 (Skip Intro) | F6 (Crash Wall) | F7 (Damage) | F8 (AI)", 400, 210);
    } else {
        // 游戏运行时显示调试常驻信息
        glColor3f(0.2f, 0.9f, 0.2f);
        drawString(GLUT_BITMAP_HELVETICA_12, "STATUS: BATTLE ONGOING (STAGE 1 FRAMEWORK ACTIVE)", 15, 700);

        std::stringstream ss;
        ss << "Hirono Pos: (" << hironoX << ", " << hironoZ << ") | Dimoo Pos: (" << dimooX << ", " << dimooZ << ")";
        glColor3f(0.9f, 0.9f, 0.9f);
        drawString(GLUT_BITMAP_HELVETICA_12, ss.str(), 15, 680);

        drawString(GLUT_BITMAP_HELVETICA_12, "Press [H] to kick desiccant bag | Press [P] to throw pamphlet", 15, 660);
        drawString(GLUT_BITMAP_HELVETICA_12, "Controls: P1: WASD | P2: ARROWS", 15, 640);
        drawString(GLUT_BITMAP_HELVETICA_12, "Press [R] to Restart", 15, 620);

        if (showColliders) {
            glColor3f(1.0f, 0.3f, 0.3f);
            drawString(GLUT_BITMAP_HELVETICA_12, "DEBUG: SHOW COLLIDERS ENABLED", 1100, 700);
        }
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void Game::handleInput(unsigned char key) {
    float step = 0.15f;
    switch (key) {
        // Player 1 (小野) 移动
        case 'a': case 'A': hironoX -= step; break;
        case 'd': case 'D': hironoX += step; break;
        case 'w': case 'W': hironoZ -= step; break;
        case 's': case 'S': hironoZ += step; break;

        // 交互测试键
        case 'h': case 'H':
            // 给干燥剂包一个随机冲量
            arena.desiccant.vx = ((rand() % 200) / 100.0f - 1.0f) * 5.0f;
            arena.desiccant.vz = ((rand() % 200) / 100.0f - 1.0f) * 5.0f;
            std::cout << "[Debug] Kicked desiccant bag!" << std::endl;
            break;

        case 'p': case 'P':
            // 抛飞说明书
            arena.testExplodeProps();
            std::cout << "[Debug] Threw pamphlet folding paper!" << std::endl;
            break;

        case 'e': case 'E':
            // 模拟撞击左壁
            arena.triggerWallShake(0.5f, true);
            camera.applyShake(0.18f);
            std::cout << "[Debug] Triggered left wall collision shake!" << std::endl;
            break;

        // 状态转换与跳过
        case ' ':
        case 13: // Enter 回车键
            if (currentState == STATE_TITLE) {
                currentState = STATE_ENTRY_ANIMATION;
                stateTimer = 0.0f;
                std::cout << "[Game] Box unboxing started!" << std::endl;
            }
            break;

        case 'r': case 'R':
            init();
            std::cout << "[Game] Restarted!" << std::endl;
            break;

        case 27: // ESC
            exit(0);
            break;
    }
}

void Game::handleSpecialInput(int key) {
    float step = 0.15f;
    switch (key) {
        // Player 2 (Dimoo) 移动
        case GLUT_KEY_LEFT:  dimooX -= step; break;
        case GLUT_KEY_RIGHT: dimooX += step; break;
        case GLUT_KEY_UP:    dimooZ -= step; break;
        case GLUT_KEY_DOWN:  dimooZ += step; break;

        // F4 ~ F8 调试热键
        case GLUT_KEY_F4:
            showColliders = !showColliders;
            std::cout << "[Debug] Toggle colliders view: " << (showColliders ? "ON" : "OFF") << std::endl;
            break;

        case GLUT_KEY_F5:
            if (currentState == STATE_TITLE || currentState == STATE_ENTRY_ANIMATION) {
                currentState = STATE_BATTLE;
                stateTimer = 0.0f;
                std::cout << "[Debug] Skipped entry animation to Battle state!" << std::endl;
            }
            break;

        case GLUT_KEY_F6:
            // 强制 P1 (小野占位符) 撞左墙
            hironoX = -BOX_WIDTH / 2.0f + 0.4f;
            arena.triggerWallShake(0.5f, true);
            camera.applyShake(0.20f);
            std::cout << "[Debug] Teleported P1 to left wall & shook screen!" << std::endl;
            break;

        case GLUT_KEY_F7:
            // 模拟大招对后盖和盒子的冲击
            arena.triggerLidShake(45.0f);
            camera.applyShake(0.35f);
            std::cout << "[Debug] Triggered ultimate impact on cardboard lid!" << std::endl;
            break;

        case GLUT_KEY_F8:
            isDummyAI = !isDummyAI;
            std::cout << "[Debug] Dummy AI: " << (isDummyAI ? "ACTIVE" : "INACTIVE") << std::endl;
            break;
    }
}
