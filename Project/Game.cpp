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
    hironoVy = 0.0f;
    hironoFacingRight = true;

    dimooX = 1.5f;   dimooY = 0.12f;  dimooZ = 0.0f; // Dimoo 稍微悬空
    dimooVy = 0.0f;
    dimooFacingRight = false;

    // HP 与回合初始设置
    hironoHp = hironoMaxHp = 100.0f;
    dimooHp = dimooMaxHp = 100.0f;
    matchTimer = 99.0f;
    matchOver = false;
}

Game::~Game() {}

void Game::init() {
    currentState = STATE_TITLE;
    stateTimer = 0.0f;
    
    hironoX = -1.5f; hironoY = 0.0f; hironoZ = 0.0f;
    hironoVy = 0.0f;
    hironoFacingRight = true;

    dimooX = 1.5f;   dimooY = 0.12f;  dimooZ = 0.0f;
    dimooVy = 0.0f;
    dimooFacingRight = false;

    hironoHp = hironoMaxHp = 100.0f;
    dimooHp = dimooMaxHp = 100.0f;
    matchTimer = 99.0f;
    matchOver = false;

    particles.clear();

    arena.init();
    camera.init();
    lighting.init();
}

void Game::spawnHitSparks(float x, float y, float z, int count, float r, float g, float b) {
    for (int i = 0; i < count; i++) {
        GameParticle p;
        p.x = x;
        p.y = y;
        p.z = z;
        // 随机扩散速度
        p.vx = ((rand() % 200) / 100.0f - 1.0f) * 2.8f;
        p.vy = ((rand() % 200) / 100.0f - 0.3f) * 3.2f; // 偏向上喷射
        p.vz = ((rand() % 200) / 100.0f - 1.0f) * 1.5f;
        p.r = r; p.g = g; p.b = b; p.a = 1.0f;
        p.size = 3.0f + (rand() % 6);
        p.maxLife = p.life = 0.3f + (rand() % 100) / 250.0f; // 0.3s ~ 0.7s
        particles.push_back(p);
    }
}

void Game::spawnDust(float x, float y, float z, int count) {
    for (int i = 0; i < count; i++) {
        GameParticle p;
        p.x = x + ((rand() % 100) / 100.0f - 0.5f) * 0.4f;
        p.y = y;
        p.z = z + ((rand() % 100) / 100.0f - 0.5f) * 0.4f;
        p.vx = ((rand() % 100) / 100.0f - 0.5f) * 0.6f;
        p.vy = 0.2f + (rand() % 100) / 250.0f; // 慢慢往上升
        p.vz = ((rand() % 100) / 100.0f - 0.5f) * 0.6f;
        p.r = 0.86f; p.g = 0.82f; p.b = 0.77f; p.a = 0.5f; // 纸屑/灰尘的温暖黄灰色
        p.size = 4.0f + (rand() % 5);
        p.maxLife = p.life = 0.4f + (rand() % 100) / 200.0f;
        particles.push_back(p);
    }
}

void Game::update(float dt) {
    arena.setHpPercentages(hironoHp, hironoMaxHp, dimooHp, dimooMaxHp);
    stateTimer += dt;

    // 更新粒子系统
    for (auto it = particles.begin(); it != particles.end(); ) {
        it->x += it->vx * dt;
        it->y += it->vy * dt;
        it->z += it->vz * dt;
        it->life -= dt;
        if (it->life <= 0.0f) {
            it = particles.erase(it);
        } else {
            it->a = it->life / it->maxLife;
            ++it;
        }
    }

    switch (currentState) {
        case STATE_TITLE:
            camera.update(dt, 0.0f, 0.0f, 0.0f, 0.0f, false);
            camera.x = 0.0f; camera.y = 1.8f; camera.z = 4.2f;
            camera.lookAtX = 0.0f; camera.lookAtY = 0.8f; camera.lookAtZ = 0.0f;
            arena.update(dt);
            break;

        case STATE_ENTRY_ANIMATION:
            camera.update(dt, hironoX, hironoY, dimooX, dimooY, false);
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
            camera.update(dt, hironoX, hironoY, dimooX, dimooY, false);
            arena.update(dt);
            if (stateTimer >= 3.0f) {
                currentState = STATE_RESULT;
                stateTimer = 0.0f;
            }
            break;

        case STATE_RESULT:
            camera.update(dt, hironoX, hironoY, dimooX, dimooY, false);
            arena.update(dt);
            break;
    }
}

void Game::updateBattle(float dt) {
    // 1. 物理计算：2D 横版重力与跳跃
    const float GRAVITY = 15.0f; // 重力加速度
    
    // Player 1 (小野) 重力与落地扬尘
    if (hironoY > 0.0f) {
        hironoVy -= GRAVITY * dt;
        hironoY += hironoVy * dt;
        if (hironoY <= 0.0f) {
            hironoY = 0.0f;
            hironoVy = 0.0f;
            spawnDust(hironoX, 0.005f, 0.0f, 8); // 落地扬尘
        }
    }

    // Player 2 (Dimoo) 重力与落地扬尘 (盘坐漂浮，高度最低为 0.12f)
    const float DIMOO_REST_Y = 0.12f;
    if (dimooY > DIMOO_REST_Y) {
        dimooVy -= GRAVITY * dt;
        dimooY += dimooVy * dt;
        if (dimooY <= DIMOO_REST_Y) {
            dimooY = DIMOO_REST_Y;
            dimooVy = 0.0f;
            spawnDust(dimooX, 0.005f, 0.0f, 8); // 落地扬尘
        }
    }

    // 2. 锁定 Z 轴，确保角色与道具在同一个剖面上
    hironoZ = 0.0f;
    dimooZ = 0.0f;

    // 3. 回合时间倒计时与 KO 检测
    if (!matchOver) {
        matchTimer -= dt;
        if (matchTimer <= 0.0f) {
            matchTimer = 0.0f;
            matchOver = true;
            currentState = STATE_VICTORY_ANIMATION;
            stateTimer = 0.0f;
        }

        if (hironoHp <= 0.0f || dimooHp <= 0.0f) {
            hironoHp = clamp(hironoHp, 0.0f, hironoMaxHp);
            dimooHp = clamp(dimooHp, 0.0f, dimooMaxHp);
            matchOver = true;
            currentState = STATE_VICTORY_ANIMATION;
            stateTimer = 0.0f;
        }
    }

    // 4. 边界限制 (防止跑出纸盒左壁与右壁)
    float borderW = BOX_WIDTH / 2.0f - 0.4f; // 扣除碰撞半径
    hironoX = clamp(hironoX, -borderW, borderW);
    dimooX = clamp(dimooX, -borderW, borderW);

    // 5. 自动更新角色朝向 (相互对视)
    hironoFacingRight = (hironoX < dimooX);
    dimooFacingRight = (dimooX < hironoX);

    // 移动时轻微几率扬尘
    if (hironoY <= 0.01f && (rand() % 100 < 5)) {
        spawnDust(hironoX, 0.005f, 0.0f, 1);
    }
    if (dimooY <= 0.121f && (rand() % 100 < 5)) {
        spawnDust(dimooX, 0.005f, 0.0f, 1);
    }

    // 6. 解决道具间的碰撞 (气泡纸，干燥剂)
    float pushHironoX = 0.0f;
    arena.resolveCollisionWithProps(hironoX, hironoY, 0.4f, pushHironoX);
    hironoX += pushHironoX;

    float pushDimooX = 0.0f;
    arena.resolveCollisionWithProps(dimooX, dimooY, 0.4f, pushDimooX);
    dimooX += pushDimooX;

    // 7. 解决两个角色之间的互相水平碰撞
    float dx = dimooX - hironoX;
    float dy = dimooY - hironoY;
    float dist = sqrt(dx*dx + dy*dy);
    float minDist = 0.8f; // 双方碰撞半径之和
    if (dist < minDist && dist > 0.001f) {
        float push = (minDist - dist) / 2.0f;
        float nx = dx / dist;
        hironoX -= nx * push;
        dimooX += nx * push;
    }

    // 再次限位
    hironoX = clamp(hironoX, -borderW, borderW);
    dimooX = clamp(dimooX, -borderW, borderW);

    // 8. 更新子模块物理与相机
    arena.update(dt);
    camera.update(dt, hironoX, hironoY, dimooX, dimooY, false);
    
    // 角色自发光点光源跟着走 (保持 Z = 0.0f)
    bool isHpLow = (hironoHp / hironoMaxHp < 0.4f || dimooHp / dimooMaxHp < 0.4f);
    lighting.updateCharacterLights(hironoX, hironoY, 0.0f, dimooX, dimooY, 0.0f, isHpLow);
}

// 绘制小野 (Hirono) 2D 纸片层级板羽人设
static void drawHironoPaper(float x, float y, float z, bool facingRight, float t, GLuint faceTex) {
    glPushMatrix();
    glTranslatef(x, y, z);
    
    if (!facingRight) {
        glScalef(-1.0f, 1.0f, 1.0f);
    }
    
    // 呼吸微缩放动画
    float scaleY = 1.0f + sin(t * 4.0f) * 0.02f;
    glScalef(1.0f, scaleY, 1.0f);
    
    // 1. 红色披风层 (Z = -0.02f)
    glColor3f(0.72f, 0.08f, 0.08f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.35f, 0.0f, -0.02f);
    glVertex3f( 0.15f, 0.0f, -0.02f);
    glVertex3f( 0.05f, 0.7f, -0.02f);
    glVertex3f(-0.25f, 0.7f, -0.02f);
    glEnd();
    
    // 2. 绿色外套身体层 (Z = 0.0f)
    glColor3f(0.15f, 0.38f, 0.22f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.2f, 0.0f, 0.0f);
    glVertex3f( 0.2f, 0.0f, 0.0f);
    glVertex3f( 0.15f, 0.5f, 0.0f);
    glVertex3f(-0.15f, 0.5f, 0.0f);
    glEnd();
    
    // 3. 脸部贴图层 (Z = 0.02f)
    if (faceTex) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, faceTex);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.3f, 0.45f, 0.02f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.3f, 0.45f, 0.02f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.3f, 1.05f, 0.02f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.3f, 1.05f, 0.02f);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    } else {
        glColor3f(0.98f, 0.88f, 0.82f);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-0.3f, 0.45f, 0.02f);
        glVertex3f( 0.3f, 0.45f, 0.02f);
        glVertex3f( 0.3f, 1.05f, 0.02f);
        glVertex3f(-0.3f, 1.05f, 0.02f);
        glEnd();
    }
    
    // 橙黄色短发层 (Z = 0.03f)
    glColor3f(0.95f, 0.72f, 0.08f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.32f, 0.95f, 0.03f);
    glVertex3f( 0.32f, 0.95f, 0.03f);
    glVertex3f( 0.25f, 1.12f, 0.03f);
    glVertex3f(-0.25f, 1.12f, 0.03f);
    glEnd();
    
    // 4. 黄色围巾飘动层 (Z = 0.04f)
    glColor3f(0.95f, 0.85f, 0.15f);
    float scarfSway = sin(t * 5.0f) * 0.05f;
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.15f, 0.48f, 0.04f);
    glVertex3f( 0.15f, 0.48f, 0.04f);
    glVertex3f( 0.15f + scarfSway, 0.58f, 0.04f);
    glVertex3f(-0.15f + scarfSway, 0.58f, 0.04f);
    glEnd();
    
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.1f, 0.48f, 0.04f);
    glVertex3f( -0.05f, 0.48f, 0.04f);
    glVertex3f( -0.3f + scarfSway, 0.35f, 0.04f);
    glVertex3f( -0.35f + scarfSway, 0.35f, 0.04f);
    glEnd();
    
    // 5. 玫瑰玻璃罩手捧层 (Z = 0.06f)
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glColor4f(0.9f, 0.95f, 1.0f, 0.55f); // 玻璃罩半透明
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.1f, 0.2f, 0.06f);
    glVertex3f( 0.1f, 0.2f, 0.06f);
    glVertex3f( 0.1f, 0.45f, 0.06f);
    glVertex3f(-0.1f, 0.45f, 0.06f);
    glEnd();
    
    // 木质底座
    glColor4f(0.4f, 0.25f, 0.15f, 1.0f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.12f, 0.17f, 0.07f);
    glVertex3f( 0.12f, 0.17f, 0.07f);
    glVertex3f( 0.12f, 0.2f, 0.07f);
    glVertex3f(-0.12f, 0.2f, 0.07f);
    glEnd();
    
    // 内部红玫瑰
    glColor4f(0.85f, 0.05f, 0.05f, 1.0f);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.03f, 0.25f, 0.065f);
    glVertex3f( 0.03f, 0.25f, 0.065f);
    glVertex3f( 0.0f,  0.33f, 0.065f);
    glEnd();
    
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    
    glPopMatrix();
}

// 绘制 Dimoo (入梦) 2D 纸片层级板羽人设
static void drawDimooPaper(float x, float y, float z, bool facingRight, float t, GLuint faceTex) {
    glPushMatrix();
    glTranslatef(x, y, z);
    
    if (!facingRight) {
        glScalef(-1.0f, 1.0f, 1.0f);
    }
    
    // 漂浮微动动画
    float floatY = sin(t * 3.0f) * 0.05f;
    glTranslatef(0.0f, floatY, 0.0f);
    
    // 呼吸缩放
    float scale = 1.0f + sin(t * 2.0f) * 0.015f;
    glScalef(scale, scale, 1.0f);
    
    // 1. 枯木藤蔓圆环背景层 (Z = -0.02f)
    glColor3f(0.32f, 0.22f, 0.12f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 24; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / 24.0f;
        float r = 0.5f + sin(t * 2.0f + i) * 0.015f;
        glVertex3f(r * cos(theta), r * sin(theta) + 0.35f, -0.02f);
    }
    glEnd();
    glLineWidth(1.0f);
    
    // 侧枝枯叶
    glColor3f(0.28f, 0.32f, 0.18f);
    glBegin(GL_TRIANGLES);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.4f, 0.7f, -0.018f);
    glVertex3f(0.48f, 0.78f, -0.018f);
    glVertex3f(0.35f, 0.82f, -0.018f);
    
    glVertex3f(-0.4f, 0.7f, -0.018f);
    glVertex3f(-0.48f, 0.78f, -0.018f);
    glVertex3f(-0.35f, 0.82f, -0.018f);
    glEnd();
    
    // 2. 盘坐睡衣身体层 (Z = 0.0f)
    glColor3f(0.9f, 0.94f, 0.98f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.25f, 0.0f, 0.0f);
    glVertex3f( 0.25f, 0.0f, 0.0f);
    glVertex3f( 0.18f, 0.45f, 0.0f);
    glVertex3f(-0.18f, 0.45f, 0.0f);
    glEnd();
    
    // 盘腿细节
    glColor3f(0.8f, 0.86f, 0.92f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.28f, 0.0f, 0.01f);
    glVertex3f( 0.28f, 0.0f, 0.01f);
    glVertex3f( 0.2f,  0.15f, 0.01f);
    glVertex3f(-0.2f,  0.15f, 0.01f);
    glEnd();
    
    // 3. 脸部贴图层 (Z = 0.02f)
    if (faceTex) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, faceTex);
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.32f, 0.35f, 0.02f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.32f, 0.35f, 0.02f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.32f, 0.95f, 0.02f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.32f, 0.95f, 0.02f);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    } else {
        glColor3f(0.98f, 0.92f, 0.90f);
        glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-0.32f, 0.35f, 0.02f);
        glVertex3f( 0.32f, 0.35f, 0.02f);
        glVertex3f( 0.32f, 0.95f, 0.02f);
        glVertex3f(-0.32f, 0.95f, 0.02f);
        glEnd();
    }
    
    // 头发蓬松层 (Z = 0.03f)
    glColor3f(0.96f, 0.92f, 0.90f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.34f, 0.85f, 0.03f);
    glVertex3f( 0.34f, 0.85f, 0.03f);
    glVertex3f( 0.28f, 1.05f, 0.03f);
    glVertex3f(-0.28f, 1.05f, 0.03f);
    glEnd();
    
    // 4. 环绕小蝴蝶层 (Z = 0.04f)
    for (int i = 0; i < 3; i++) {
        float phase = t * 3.0f + i * 2.0f * (float)M_PI / 3.0f;
        float bx = cos(phase) * 0.4f;
        float by = sin(phase) * 0.3f + 0.45f;
        float bz = 0.04f;
        
        float wingFlap = sin(t * 20.0f) * 0.06f;
        
        glPushMatrix();
        glTranslatef(bx, by, bz);
        
        glColor3f(0.35f, 0.4f, 0.28f);
        glBegin(GL_TRIANGLES);
        glNormal3f(0.0f, 0.0f, 1.0f);
        // Left wing
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(-0.06f, 0.06f + wingFlap, 0.0f);
        glVertex3f(-0.06f, -0.04f, 0.0f);
        
        // Right wing
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.06f, 0.06f + wingFlap, 0.0f);
        glVertex3f(0.06f, -0.04f, 0.0f);
        glEnd();
        
        glPopMatrix();
    }
    
    glPopMatrix();
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

    // 在 BATTLE 或其他对战状态下，绘制 2D 纸片板羽角色与阴影
    if (currentState != STATE_TITLE) {
        float t = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
        
        // 绘制阴影
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        
        // P1 阴影
        glColor4f(0.0f, 0.0f, 0.0f, 0.13f);
        glPushMatrix();
        glTranslatef(hironoX, 0.001f, 0.0f);
        glScalef(1.0f, 0.3f, 1.0f);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 16; i++) {
            float th = 2.0f * (float)M_PI * (float)i / 16.0f;
            glVertex3f(0.35f * cos(th), 0.0f, 0.18f * sin(th));
        }
        glEnd();
        glPopMatrix();

        // P2 阴影 (随高度变化缩放和渐变)
        float floatY = sin(t * 3.0f) * 0.05f;
        float currentDimooY = dimooY + floatY;
        float shadowAlpha = 0.13f * (1.0f - currentDimooY * 0.5f);
        if (shadowAlpha < 0.035f) shadowAlpha = 0.035f;
        float shadowSize = 0.35f * (1.0f - currentDimooY * 0.3f);
        if (shadowSize < 0.15f) shadowSize = 0.15f;
        
        glColor4f(0.0f, 0.0f, 0.0f, shadowAlpha);
        glPushMatrix();
        glTranslatef(dimooX, 0.001f, 0.0f);
        glScalef(1.0f, 0.3f, 1.0f);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 16; i++) {
            float th = 2.0f * (float)M_PI * (float)i / 16.0f;
            glVertex3f(shadowSize * cos(th), 0.0f, 0.18f * sin(th));
        }
        glEnd();
        glPopMatrix();

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);

        // 绘制纸片角色 (关闭背面剔除，防止翻转面消失)
        glDisable(GL_CULL_FACE);
        
        // Player 1: Hirono 2D Paper
        drawHironoPaper(hironoX, hironoY, hironoZ, hironoFacingRight, t, arena.hironoFaceTex);

        // Player 2: Dimoo 2D Paper
        drawDimooPaper(dimooX, dimooY, dimooZ, dimooFacingRight, t, arena.dimooFaceTex);
        
        glEnable(GL_CULL_FACE);
    }

    // 绘制 3D 空间粒子 (火花、落地灰尘)
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    for (const auto& p : particles) {
        glColor4f(p.r, p.g, p.b, p.a);
        glPointSize(p.size);
        glBegin(GL_POINTS);
        glVertex3f(p.x, p.y, p.z);
        glEnd();
    }
    glPointSize(1.0f);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

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
        glTranslatef(arena.desiccant.x, arena.desiccant.y - 0.05f, 0.0f);
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
        drawString(GLUT_BITMAP_HELVETICA_12, "Player 1 (Hirono): A/D (Walk), W (Jump), J/K/L (Attack/Skill/Ult) | Player 2 (Dimoo): Left/Right/Up, 1/2/3 (Attack/Skill/Ult)", 200, 240);
        drawString(GLUT_BITMAP_HELVETICA_12, "Debug Keys: F4 (Colliders) | F5 (Skip Intro) | F6 (Crash Wall) | F7 (Damage P2) | F8 (AI) | F9 (Free Cam)", 380, 210);
    } else {
        // 绘制血条背景边框 (KOF 街机风格)
        glColor3f(0.12f, 0.12f, 0.12f);
        glBegin(GL_QUADS);
        // P1 血条黑边
        glVertex2f(45, 640); glVertex2f(505, 640);
        glVertex2f(505, 675); glVertex2f(45, 675);
        // P2 血条黑边
        glVertex2f(775, 640); glVertex2f(1235, 640);
        glVertex2f(1235, 675); glVertex2f(775, 675);
        glEnd();

        // 绘制空血红底
        glColor3f(0.5f, 0.1f, 0.1f);
        glBegin(GL_QUADS);
        glVertex2f(50, 645); glVertex2f(500, 645);
        glVertex2f(500, 670); glVertex2f(50, 670);
        
        glVertex2f(780, 645); glVertex2f(1230, 645);
        glVertex2f(1230, 670); glVertex2f(780, 670);
        glEnd();

        // 绘制当前绿-黄-红渐变血条
        float h1Pct = hironoHp / hironoMaxHp;
        float h2Pct = dimooHp / dimooMaxHp;

        glBegin(GL_QUADS);
        // P1 血条 fill (左缩)
        glColor3f(1.0f - h1Pct, h1Pct, 0.0f);
        glVertex2f(50, 645); glVertex2f(50 + 450.0f * h1Pct, 645);
        glVertex2f(50 + 450.0f * h1Pct, 670); glVertex2f(50, 670);
        
        // P2 血条 fill (右缩)
        glColor3f(1.0f - h2Pct, h2Pct, 0.0f);
        glVertex2f(1230 - 450.0f * h2Pct, 645); glVertex2f(1230, 645);
        glVertex2f(1230, 670); glVertex2f(1230 - 450.0f * h2Pct, 670);
        glEnd();

        // 绘制 P1 & P2 盲盒原画小头像
        if (arena.hironoFaceTex) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, arena.hironoFaceTex);
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(5, 638);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(40, 638);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(40, 675);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(5, 675);
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }
        
        if (arena.dimooFaceTex) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, arena.dimooFaceTex);
            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(1240, 638);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(1275, 638);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(1275, 675);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(1240, 675);
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }

        // 绘制中央倒计时框
        glColor3f(0.15f, 0.15f, 0.15f);
        glBegin(GL_QUADS);
        glVertex2f(615, 635); glVertex2f(665, 635);
        glVertex2f(665, 675); glVertex2f(615, 675);
        glEnd();

        glColor3f(1.0f, 0.8f, 0.0f);
        std::stringstream ssTime;
        ssTime << (int)matchTimer;
        drawString(GLUT_BITMAP_HELVETICA_18, ssTime.str(), (matchTimer >= 10.0f) ? 623 : 633, 648);

        // 名字标签
        glColor3f(1.0f, 1.0f, 1.0f);
        drawString(GLUT_BITMAP_HELVETICA_12, "P1: HIRONO", 55, 680);
        drawString(GLUT_BITMAP_HELVETICA_12, "P2: DIMOO", 1150, 680);

        // 调试常驻信息与浮空相机提示
        if (camera.freeCam) {
            glColor3f(0.9f, 0.9f, 0.1f);
            drawString(GLUT_BITMAP_HELVETICA_12, "STATUS: FREE CAMERA ACTIVE (F9 to Exit)", 15, 700);
        } else {
            glColor3f(0.2f, 0.9f, 0.2f);
            drawString(GLUT_BITMAP_HELVETICA_12, "STATUS: BATTLE ONGOING (F9 for Free Cam)", 15, 700);
        }

        std::stringstream ss;
        ss << "Hirono Pos: (" << hironoX << ", " << hironoY << ") | Dimoo Pos: (" << dimooX << ", " << dimooY << ")";
        glColor3f(0.9f, 0.9f, 0.9f);
        drawString(GLUT_BITMAP_HELVETICA_12, ss.str(), 15, 15);

        if (camera.freeCam) {
            drawString(GLUT_BITMAP_HELVETICA_12, "Free Cam Controls: WASD/Arrows (Orbit), Q/E (Zoom In/Out), F/V (Up/Down), R (Reset)", 15, 35);
            drawString(GLUT_BITMAP_HELVETICA_12, "Inspect the box from any angle using your keyboard controls!", 15, 55);
        } else {
            drawString(GLUT_BITMAP_HELVETICA_12, "Press [H] to kick desiccant bag | Press [P] to throw pamphlet", 15, 35);
            drawString(GLUT_BITMAP_HELVETICA_12, "Controls: P1: A/D/W/J/K/L (Combat) | P2: Left/Right/Up/1/2/3 (Combat)", 15, 55);
        }

        if (showColliders) {
            glColor3f(1.0f, 0.3f, 0.3f);
            drawString(GLUT_BITMAP_HELVETICA_12, "DEBUG: SHOW COLLIDERS ENABLED", 1100, 700);
        }

        // KO 结算画面
        if (matchOver) {
            // 暗色半透明罩层
            glColor4f(0.0f, 0.0f, 0.0f, 0.55f);
            glEnable(GL_BLEND);
            glBegin(GL_QUADS);
            glVertex2f(0, 0); glVertex2f(1280, 0);
            glVertex2f(1280, 720); glVertex2f(0, 720);
            glEnd();
            glDisable(GL_BLEND);
            
            // K.O. 街机红黄标
            glColor3f(1.0f, 0.2f, 0.2f);
            drawString(GLUT_BITMAP_TIMES_ROMAN_24, "K. O.", 610, 380);
            
            glColor3f(0.95f, 0.9f, 0.5f);
            std::string winStr = (hironoHp > dimooHp) ? "HIRONO WINS!" : "DIMOO WINS!";
            if (hironoHp == dimooHp) winStr = "DRAW MATCH!";
            drawString(GLUT_BITMAP_HELVETICA_18, winStr, 570, 320);
            
            glColor3f(0.7f, 0.7f, 0.7f);
            drawString(GLUT_BITMAP_HELVETICA_12, "Press [R] to Restart Match", 575, 275);
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
    // Intercept keyboard inputs if free camera mode is enabled
    if (camera.freeCam) {
        float rotStep = 3.0f;
        float zoomStep = 0.15f;
        switch (key) {
            case 'a': case 'A': camera.yaw -= rotStep; return;
            case 'd': case 'D': camera.yaw += rotStep; return;
            case 'w': case 'W': camera.pitch += rotStep; return;
            case 's': case 'S': camera.pitch -= rotStep; return;
            case 'q': case 'Q': camera.radius -= zoomStep; return;
            case 'e': case 'E': camera.radius += zoomStep; return;
            case 'f': case 'F': camera.centerY += 0.1f; return;
            case 'v': case 'V': camera.centerY -= 0.1f; return;
            case 'r': case 'R': camera.init(); return;
            case 27: exit(0); return;
        }
    }

    float step = 0.15f;
    switch (key) {
        // Player 1 (小野) 移动与跳跃
        case 'a': case 'A': hironoX -= step; break;
        case 'd': case 'D': hironoX += step; break;
        case 'w': case 'W': 
            if (hironoY <= 0.001f) {
                hironoVy = 5.5f; // 给予向上的速度跳起
            }
            break;

        // Player 1 (小野) 战斗招式键
        case 'j': case 'J':
            if (currentState == STATE_BATTLE && !matchOver) {
                if (std::abs(hironoX - dimooX) < 1.1f && std::abs(hironoY - dimooY) < 1.0f) {
                    dimooHp -= 8.0f;
                    spawnHitSparks(dimooX, dimooY + 0.5f, 0.0f, 15, 1.0f, 0.2f, 0.2f);
                    camera.applyShake(0.18f);
                    std::cout << "[Combat] Hirono hit Dimoo! Dimoo HP: " << dimooHp << std::endl;
                } else {
                    spawnHitSparks(hironoX + (hironoFacingRight ? 0.5f : -0.5f), hironoY + 0.5f, 0.0f, 3, 0.95f, 0.72f, 0.08f);
                }
            }
            break;

        case 'k': case 'K':
            if (currentState == STATE_BATTLE && !matchOver) {
                if (std::abs(hironoX - dimooX) < 2.0f && std::abs(hironoY - dimooY) < 1.2f) {
                    dimooHp -= 16.0f;
                    spawnHitSparks(dimooX, dimooY + 0.5f, 0.0f, 25, 1.0f, 0.85f, 0.2f);
                    camera.applyShake(0.26f);
                    std::cout << "[Combat] Hirono Golden Star hit Dimoo! Dimoo HP: " << dimooHp << std::endl;
                } else {
                    spawnHitSparks(hironoX + (hironoFacingRight ? 0.8f : -0.8f), hironoY + 0.5f, 0.0f, 5, 1.0f, 0.85f, 0.2f);
                }
            }
            break;

        case 'l': case 'L':
            if (currentState == STATE_BATTLE && !matchOver) {
                if (std::abs(hironoX - dimooX) < 3.0f && std::abs(hironoY - dimooY) < 2.0f) {
                    dimooHp -= 30.0f;
                    spawnHitSparks(dimooX, dimooY + 0.5f, 0.0f, 40, 0.95f, 0.95f, 1.0f);
                    camera.applyShake(0.45f);
                    arena.triggerLidShake(45.0f);
                    std::cout << "[Combat] Hirono ultimate hit Dimoo! Dimoo HP: " << dimooHp << std::endl;
                }
            }
            break;

        // Player 2 (Dimoo) 战斗招式键 (主键盘 1 / 2 / 3)
        case '1':
            if (currentState == STATE_BATTLE && !matchOver) {
                if (std::abs(dimooX - hironoX) < 1.1f && std::abs(dimooY - hironoY) < 1.0f) {
                    hironoHp -= 8.0f;
                    spawnHitSparks(hironoX, hironoY + 0.5f, 0.0f, 15, 0.3f, 0.6f, 1.0f);
                    camera.applyShake(0.18f);
                    std::cout << "[Combat] Dimoo hit Hirono! Hirono HP: " << hironoHp << std::endl;
                } else {
                    spawnHitSparks(dimooX + (dimooFacingRight ? 0.5f : -0.5f), dimooY + 0.5f, 0.0f, 3, 0.3f, 0.6f, 1.0f);
                }
            }
            break;

        case '2':
            if (currentState == STATE_BATTLE && !matchOver) {
                if (std::abs(dimooX - hironoX) < 2.0f && std::abs(dimooY - hironoY) < 1.2f) {
                    hironoHp -= 16.0f;
                    spawnHitSparks(hironoX, hironoY + 0.5f, 0.0f, 25, 0.2f, 0.9f, 0.9f);
                    camera.applyShake(0.26f);
                    std::cout << "[Combat] Dimoo Sleep ripple hit Hirono! Hirono HP: " << hironoHp << std::endl;
                } else {
                    spawnHitSparks(dimooX + (dimooFacingRight ? 0.8f : -0.8f), dimooY + 0.5f, 0.0f, 5, 0.2f, 0.9f, 0.9f);
                }
            }
            break;

        case '3':
            if (currentState == STATE_BATTLE && !matchOver) {
                if (std::abs(dimooX - hironoX) < 3.0f && std::abs(dimooY - hironoY) < 2.0f) {
                    hironoHp -= 30.0f;
                    spawnHitSparks(hironoX, hironoY + 0.5f, 0.0f, 40, 0.9f, 0.95f, 1.0f);
                    camera.applyShake(0.45f);
                    arena.triggerLidShake(45.0f);
                    std::cout << "[Combat] Dimoo ultimate hit Hirono! Hirono HP: " << hironoHp << std::endl;
                }
            }
            break;

        // 交互测试键
        case 'h': case 'H':
            // 给干燥剂包一个随机冲量 (仅横向 vx 运动)
            arena.desiccant.vx = ((rand() % 200) / 100.0f - 1.0f) * 5.0f;
            arena.desiccant.popped = true;
            std::cout << "[Debug] Kicked desiccant bag!" << std::endl;
            break;

        case 'p': case 'P':
            // 抛飞说明书 (X-Y 平面内飞行)
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
    // Intercept special keyboard inputs if free camera mode is enabled
    if (key == GLUT_KEY_F9) {
        camera.freeCam = !camera.freeCam;
        std::cout << "[Camera] Toggle Free Camera: " << (camera.freeCam ? "ON" : "OFF") << std::endl;
        return;
    }

    if (camera.freeCam) {
        float rotStep = 3.0f;
        switch (key) {
            case GLUT_KEY_LEFT:  camera.yaw -= rotStep; return;
            case GLUT_KEY_RIGHT: camera.yaw += rotStep; return;
            case GLUT_KEY_UP:    camera.pitch += rotStep; return;
            case GLUT_KEY_DOWN:  camera.pitch -= rotStep; return;
        }
    }

    float step = 0.15f;
    switch (key) {
        // Player 2 (Dimoo) 移动与跳跃
        case GLUT_KEY_LEFT:  dimooX -= step; break;
        case GLUT_KEY_RIGHT: dimooX += step; break;
        case GLUT_KEY_UP:
            if (dimooY <= 0.121f) {
                dimooVy = 5.5f; // 给予向上的速度跳起
            }
            break;

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
            // 强制 P1 (小野占位贴图) 撞左墙
            hironoX = -BOX_WIDTH / 2.0f + 0.4f;
            arena.triggerWallShake(0.5f, true);
            camera.applyShake(0.20f);
            std::cout << "[Debug] Teleported P1 to left wall & shook screen!" << std::endl;
            break;

        case GLUT_KEY_F7:
            // F7 修改为一键伤害调试键 (伤害 Dimoo P2 并产生受击火花)
            if (currentState == STATE_BATTLE && !matchOver) {
                dimooHp -= 15.0f;
                spawnHitSparks(dimooX, dimooY + 0.5f, 0.0f, 20, 1.0f, 0.0f, 0.0f);
                camera.applyShake(0.25f);
                std::cout << "[Debug] Dealt 15% damage to Dimoo (P2)! Dimoo HP: " << dimooHp << std::endl;
            }
            break;

        case GLUT_KEY_F8:
            isDummyAI = !isDummyAI;
            std::cout << "[Debug] Dummy AI: " << (isDummyAI ? "ACTIVE" : "INACTIVE") << std::endl;
            break;
    }
}
