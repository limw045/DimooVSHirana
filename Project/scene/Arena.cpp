#include "Arena.h"
#include "../CGImageLoader.hpp"
#include <iostream>

// 纹理加载辅助函数
static GLuint loadTextureHelper(const char* filename) {
    MyImage img;
    img.loadJPG((char*)filename);
    if (!img.buffer) {
        std::cerr << "[Arena] Failed to load JPG texture: " << filename << std::endl;
        return 0;
    }
    
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, img.width, img.height, GL_RGB, GL_UNSIGNED_BYTE, img.buffer);
    
    std::cout << "[Arena] Successfully loaded texture: " << filename << " (" << img.width << "x" << img.height << ")" << std::endl;
    return tex;
}

// 辅助函数：绘制实心圆（用于程序化墙壁图案）
static void drawSolidCircle(float x, float y, float r, int segments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < segments; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / (float)segments;
        glVertex2f(x + r * cos(theta), y + r * sin(theta));
    }
    glEnd();
}

// 声明程序化图案绘制函数，防止未定义先调用
void drawRosePattern(float wallX, float wallHeight);

Arena::Arena() {
    leftWallOffset = 0.0f;
    rightWallOffset = 0.0f;
    leftWallShakeTime = 0.0f;
    rightWallShakeTime = 0.0f;

    cardboardTex = 0;
    woodTex = 0;
    roseLabelTex = 0;
    starrySkyTex = 0;

    // 在 Z=0 平面上沿 X 轴排布一排 2D 气泡纸单元作为格斗垫
    float startX = -BOX_WIDTH / 2.0f + 1.0f;
    for (int c = 0; c < 12; ++c) {
        BubbleCell cell;
        cell.x = startX + c * 0.35f;
        cell.y = 0.005f; // 贴地高度
        cell.pressDepth = 0.0f;
        cell.pressVelocity = 0.0f;
        cell.popped = false;
        bubbles.push_back(cell);
    }
}

Arena::~Arena() {
    if (cardboardTex) glDeleteTextures(1, &cardboardTex);
    if (woodTex) glDeleteTextures(1, &woodTex);
    if (roseLabelTex) glDeleteTextures(1, &roseLabelTex);
    if (starrySkyTex) glDeleteTextures(1, &starrySkyTex);
}

void Arena::init() {
    cardboardTex = loadTextureHelper("images/cardboard.jpg");
    woodTex = loadTextureHelper("images/wood.jpg");
    roseLabelTex = loadTextureHelper("images/rose_label.jpg");
    starrySkyTex = loadTextureHelper("images/starry_sky.jpg");
    hironoFaceTex = loadTextureHelper("images/hirono_face.jpg");
    dimooFaceTex = loadTextureHelper("images/dimoo_face.jpg");
}

void Arena::update(float dt) {
    // 1. 物理计算：3D 盒盖震动回弹
    float spring = -lid.springK * (lid.angle - lid.restAngle);
    float damp = -lid.damping * lid.velocity;
    lid.velocity += (spring + damp) * dt;
    lid.angle += lid.velocity * dt;
    lid.angle = clamp(lid.angle, 90.0f, 180.0f);

    // 2. 物理计算：3D 左右壁受击震动
    if (leftWallShakeTime > 0.0f) {
        leftWallShakeTime -= dt;
        leftWallOffset = exp(-4.0f * (0.5f - leftWallShakeTime)) * sin(25.0f * leftWallShakeTime) * 0.12f;
        if (leftWallShakeTime <= 0.0f) leftWallOffset = 0.0f;
    }
    if (rightWallShakeTime > 0.0f) {
        rightWallShakeTime -= dt;
        rightWallOffset = exp(-4.0f * (0.5f - rightWallShakeTime)) * sin(25.0f * rightWallShakeTime) * 0.12f;
        if (rightWallShakeTime <= 0.0f) rightWallOffset = 0.0f;
    }

    // 3. 物理计算：2D 气泡垫踩踏恢复
    for (size_t i = 0; i < bubbles.size(); ++i) {
        if (bubbles[i].popped) continue;
        float bSpring = -25.0f * bubbles[i].pressDepth;
        float bDamp = -6.0f * bubbles[i].pressVelocity;
        bubbles[i].pressVelocity += (bSpring + bDamp) * dt;
        bubbles[i].pressDepth += bubbles[i].pressVelocity * dt;
        bubbles[i].pressDepth = clamp(bubbles[i].pressDepth, 0.0f, 1.0f);
    }

    // 4. 物理计算：2D 折纸说明书在 X-Y 平面飞行物理 (Z=0.0)
    if (pamphlet.isFlying) {
        pamphlet.x += pamphlet.vx * dt;
        pamphlet.y += pamphlet.vy * dt;
        pamphlet.vy -= 5.8f * dt; // 重力加速度

        pamphlet.rot += pamphlet.rotV * dt;

        // 飞出包装时纸张逐渐展开
        pamphlet.foldAngle += 0.8f * dt;
        if (pamphlet.foldAngle > 1.0f) pamphlet.foldAngle = 1.0f;

        // 落地碰撞检测 (地面高度 Y = 0.1)
        if (pamphlet.y <= 0.1f) {
            pamphlet.y = 0.1f;
            pamphlet.vy = 0.0f;
            pamphlet.vx = 0.0f;
            pamphlet.rotV = 0.0f;
            pamphlet.rot = 0.0f; // 平躺在地上
            pamphlet.isFlying = false;
        }
    }

    // 5. 物理计算：2D 干燥剂包在地表 X 轴的阻尼滑行 (Y=0.12f, Z=0.0)
    float speedSq = desiccant.vx * desiccant.vx;
    if (speedSq > 0.0001f) {
        float speed = sqrt(speedSq);
        float friction = 1.6f; // 滑动摩擦阻力
        desiccant.vx -= (desiccant.vx / speed) * friction * dt;
        desiccant.x += desiccant.vx * dt;

        // 左右纸壁碰撞反弹
        float limitX = BOX_WIDTH / 2.0f - 0.25f;
        if (desiccant.x > limitX) {
            desiccant.x = limitX;
            desiccant.vx *= -0.6f; // 撞墙反弹
            triggerWallShake(0.3f, false);
        }
        if (desiccant.x < -limitX) {
            desiccant.x = -limitX;
            desiccant.vx *= -0.6f;
            triggerWallShake(0.3f, true);
        }
    }
}

void Arena::triggerWallShake(float intensity, bool leftWall) {
    if (leftWall) leftWallShakeTime = 0.5f;
    else rightWallShakeTime = 0.5f;
}

void Arena::triggerLidShake(float intensity) {
    lid.velocity += intensity;
}

void Arena::testExplodeProps() {
    // 激活 2D 说明书抛飞
    if (!pamphlet.isFlying) {
        pamphlet.isFlying = true;
        pamphlet.y = 0.3f;
        pamphlet.vy = 2.5f + (rand() % 100) / 100.0f;
        pamphlet.vx = ((rand() % 200) / 100.0f - 1.0f) * 1.8f;
        pamphlet.rotV = 360.0f * ((rand() % 2 == 0) ? 1.0f : -1.0f);
        pamphlet.foldAngle = 0.0f;
    }
    // 给 2D 干燥剂包左右滑行冲量
    desiccant.vx = ((rand() % 200) / 100.0f - 1.0f) * 5.0f;
    desiccant.popped = false;
}

void Arena::resolveCollisionWithProps(float charX, float charY, float radius, float& outPushX) {
    outPushX = 0.0f;

    // 1. 2D 气泡纸踩爆判定 (角色站在地表且进入X区间)
    for (size_t i = 0; i < bubbles.size(); ++i) {
        if (bubbles[i].popped) continue;
        float dx = bubbles[i].x - charX;
        // 踩踏高度检测 Y < 0.1f 判定为脚踩
        if (std::abs(dx) < radius && charY < 0.1f) {
            bubbles[i].pressDepth = 0.85f;
            bubbles[i].pressVelocity = -3.5f;
            
            // 3% 概率踩爆
            if ((rand() % 100) < 3) {
                bubbles[i].popped = true;
            }
        }
    }

    // 2. 2D 干燥剂包与角色在 X 轴上的碰撞排斥与能量传递
    float dx = desiccant.x - charX;
    float dy = desiccant.y - charY;
    float dist = sqrt(dx*dx + dy*dy);
    float minDist = radius + 0.16f; // 2D 包裹半径
    if (dist < minDist && dist > 0.001f) {
        float push = minDist - dist;
        // 主要是横向推开力
        outPushX = -(dx / dist) * push * 0.1f; // 角色被轻微阻挡
        desiccant.x += (dx / dist) * push * 0.9f; // 干燥剂包被推开
        desiccant.vx += (dx / dist) * 3.5f; // 获得滑行初速度
    }
}

void Arena::drawTexturedQuad(float x1, float y1, float z1,
                             float x2, float y2, float z2,
                             float x3, float y3, float z3,
                             float x4, float y4, float z4,
                             float uMax, float vMax) {
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x1, y1, z1);
    glTexCoord2f(uMax, 0.0f); glVertex3f(x2, y2, z2);
    glTexCoord2f(uMax, vMax); glVertex3f(x3, y3, z3);
    glTexCoord2f(0.0f, vMax); glVertex3f(x4, y4, z4);
    glEnd();
}

void Arena::drawBox() {
    float halfW = BOX_WIDTH / 2.0f;
    float halfD = BOX_DEPTH / 2.0f;
    float height = BOX_HEIGHT;

    // --- 1. 盒底 (地板 - 贴图暖卡纸) ---
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cardboardTex);
    GLfloat mat_floor_specular[] = { 0.05f, 0.05f, 0.05f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_floor_specular);
    glColor3f(0.96f, 0.95f, 0.93f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfW, 0.0f,  halfD);
    glTexCoord2f(2.0f, 0.0f); glVertex3f( halfW, 0.0f,  halfD);
    glTexCoord2f(2.0f, 2.0f); glVertex3f( halfW, 0.0f, -halfD);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(-halfW, 0.0f, -halfD);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // 绘制压痕折叠折线与圆形 POP MART 水印 (GDD 2.0 规格)
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glColor4f(0.85f, 0.83f, 0.80f, 0.5f);
    
    // 中心圆形水印 (Z轴稍高防深度遮挡)
    glPushMatrix();
    glTranslatef(0.0f, 0.002f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 40; i++) {
        float theta = 2.0f * (float)M_PI * (float)i / 40.0f;
        glVertex2f(0.65f * cos(theta), 0.65f * sin(theta));
    }
    glEnd();
    glLineWidth(1.0f);
    glPopMatrix();
    
    // 对角折痕线
    glBegin(GL_LINES);
    glVertex3f(-halfW, 0.002f,  halfD); glVertex3f(-halfW + 0.8f, 0.002f,  halfD - 0.8f);
    glVertex3f( halfW, 0.002f,  halfD); glVertex3f( halfW - 0.8f, 0.002f,  halfD - 0.8f);
    glVertex3f( halfW, 0.002f, -halfD); glVertex3f( halfW - 0.8f, 0.002f, -halfD + 0.8f);
    glVertex3f(-halfW, 0.002f, -halfD); glVertex3f(-halfW + 0.8f, 0.002f, -halfD + 0.8f);
    glEnd();
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    // --- 2. 左壁 (小野的主场 - 深绿底 + 玫瑰/星光装饰) ---
    glPushMatrix();
    glTranslatef(leftWallOffset, 0, 0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cardboardTex);
    glColor3f(0.15f, 0.38f, 0.22f);
    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfW, 0.0f,   halfD);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(-halfW, 0.0f,  -halfD);
    glTexCoord2f(2.0f, 2.0f); glVertex3f(-halfW, height, -halfD);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(-halfW, height,  halfD);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_LIGHTING);
    drawRosePattern(-halfW, height);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // --- 3. 右壁 (Dimoo的主场 - 浅蓝底 + 藤蔓纹饰) ---
    glPushMatrix();
    glTranslatef(rightWallOffset, 0, 0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cardboardTex);
    glColor3f(0.85f, 0.93f, 0.96f);
    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(halfW, 0.0f,  -halfD);
    glTexCoord2f(2.0f, 0.0f); glVertex3f(halfW, 0.0f,   halfD);
    glTexCoord2f(2.0f, 2.0f); glVertex3f(halfW, height,  halfD);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(halfW, height, -halfD);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_LIGHTING);
    glColor4f(0.35f, 0.45f, 0.38f, 0.8f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 20; i++) {
        float theta = (float)i * 0.15f;
        float zCoord = -halfD + (float)i * 0.3f;
        float yCoord = 1.0f + sin(theta) * 0.8f + (float)i * 0.1f;
        glVertex3f(halfW - 0.01f, yCoord, zCoord);
    }
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // --- 4. 后壁 (包装规格条码信息面) ---
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cardboardTex);
    glColor3f(0.92f, 0.90f, 0.88f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfW, 0.0f,  -halfD);
    glTexCoord2f(2.0f, 0.0f); glVertex3f( halfW, 0.0f,  -halfD);
    glTexCoord2f(2.0f, 2.0f); glVertex3f( halfW, height, -halfD);
    glTexCoord2f(0.0f, 2.0f); glVertex3f(-halfW, height, -halfD);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glDisable(GL_LIGHTING);
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex3f(-1.5f, 0.8f, -halfD + 0.01f);
    glVertex3f(-0.5f, 0.8f, -halfD + 0.01f);
    glVertex3f(-0.5f, 1.8f, -halfD + 0.01f);
    glVertex3f(-1.5f, 1.8f, -halfD + 0.01f);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (int i = 0; i < 12; i++) {
        float xOffset = -1.45f + i * 0.08f;
        glVertex3f(xOffset, 0.9f, -halfD + 0.02f);
        glVertex3f(xOffset, 1.7f, -halfD + 0.02f);
    }
    glEnd();
    glLineWidth(1.0f);
    glEnable(GL_LIGHTING);
}

void Arena::drawLid() {
    float halfW = BOX_WIDTH / 2.0f;
    float halfD = BOX_DEPTH / 2.0f;
    float height = BOX_HEIGHT;

    glPushMatrix();
    glTranslatef(0.0f, height, -halfD);
    glRotatef(lid.angle, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, halfD);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cardboardTex);

    glColor3f(0.82f, 0.76f, 0.70f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    drawTexturedQuad(-halfW, 0.0f,  halfD,
                      halfW, 0.0f,  halfD,
                      halfW, 0.0f, -halfD,
                     -halfW, 0.0f, -halfD, 1.0f, 1.0f);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void Arena::drawBubbleWrap() {
    // 渲染 2D 气泡纸条 (在 Z = 0 附近横跨一小片区域)
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    // 半透明背景垫 (Z = 0 平面，厚度极薄)
    glColor4f(0.75f, 0.92f, 0.78f, 0.45f);
    float length = 1.0f;
    float posX = -1.0f;

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(posX - length, 0.006f,  0.15f);
    glVertex3f(posX + length, 0.006f,  0.15f);
    glVertex3f(posX + length, 0.006f, -0.15f);
    glVertex3f(posX - length, 0.006f, -0.15f);
    glEnd();

    // 绘制 2D 半透明气泡圆圈 (垂直朝向摄像机，扁平椭圆)
    glColor4f(0.85f, 0.97f, 0.92f, 0.65f);
    for (size_t i = 0; i < bubbles.size(); ++i) {
        if (bubbles[i].popped) continue;
        glPushMatrix();
        glTranslatef(bubbles[i].x, bubbles[i].y, 0.0f);
        glScalef(1.0f, 0.8f * (1.0f - bubbles[i].pressDepth * 0.85f), 1.0f);
        
        // 用扁球体模拟 2D 气泡
        glutSolidSphere(0.06, 8, 8);
        glPopMatrix();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void Arena::drawPamphlet() {
    // 2D 纸片说明书：平行于摄像机的平直 quad 面片，可在 X-Y 平面内任意飞起与旋转 (Z = 0.0)
    glPushMatrix();
    glTranslatef(pamphlet.x, pamphlet.y, 0.0f); // 坐标完全在 Z=0 轴上
    glRotatef(pamphlet.rot, 0.0f, 0.0f, 1.0f);  // 2D 自旋旋转

    glColor3f(0.95f, 0.95f, 0.95f);
    
    // 绘制一个带中折线的 2D 扁平纸板 (0.45 x 0.3)
    float w = 0.22f, h = 0.15f;
    
    // 开启双面光照或者禁用剔除以保证背面可见
    glDisable(GL_CULL_FACE);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w, -h, 0.0f);
    glVertex3f( w, -h, 0.0f);
    glVertex3f( w,  h, 0.0f);
    glVertex3f(-w,  h, 0.0f);
    glEnd();
    glEnable(GL_CULL_FACE);

    // 绘制 2D 纸片边缘的细节笔触线
    glDisable(GL_LIGHTING);
    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-w, -h, 0.001f);
    glVertex3f( w, -h, 0.001f);
    glVertex3f( w,  h, 0.001f);
    glVertex3f(-w,  h, 0.001f);
    glEnd();
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void Arena::drawDesiccant() {
    // 2D 干燥剂包：站立在 Z = 0 平面上的纸片包 (X-Y 平面)
    glPushMatrix();
    glTranslatef(desiccant.x, desiccant.y, 0.0f); // 滑动位置完全锁定在 Z = 0

    glColor3f(0.88f, 0.88f, 0.88f);
    float w = 0.16f, h = 0.10f;

    glDisable(GL_CULL_FACE);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w, -h, 0.0f);
    glVertex3f( w, -h, 0.0f);
    glVertex3f( w,  h, 0.0f);
    glVertex3f(-w,  h, 0.0f);
    glEnd();
    glEnable(GL_CULL_FACE);

    // 绘制干燥剂包文字图案的简线条
    glDisable(GL_LIGHTING);
    glColor3f(0.3f, 0.3f, 0.5f);
    glBegin(GL_LINES);
    // 模拟横线条文字
    glVertex3f(-w + 0.04f, 0.02f, 0.001f);  glVertex3f(w - 0.04f, 0.02f, 0.001f);
    glVertex3f(-w + 0.04f, -0.02f, 0.001f); glVertex3f(w - 0.04f, -0.02f, 0.001f);
    // 封口折痕
    glVertex3f(-w,  h - 0.02f, 0.001f); glVertex3f(w,  h - 0.02f, 0.001f);
    glVertex3f(-w, -h + 0.02f, 0.001f); glVertex3f(w, -h + 0.02f, 0.001f);
    glEnd();
    glEnable(GL_LIGHTING);

    glPopMatrix();
}

void Arena::drawOpaque() {
    drawBox();
    drawLid();
    drawPamphlet();
    drawDesiccant();
}

void Arena::drawTransparent() {
    // 1. 绘制前壁
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    float halfW = BOX_WIDTH / 2.0f;
    float halfD = BOX_DEPTH / 2.0f;
    float height = BOX_HEIGHT;

    glColor4f(0.8f, 0.82f, 0.85f, 0.15f);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-halfW, 0.0f,   halfD);
    glVertex3f( halfW, 0.0f,   halfD);
    glVertex3f( halfW, height,  halfD);
    glVertex3f(-halfW, height,  halfD);
    glEnd();

    // 2. 绘制气泡纸
    drawBubbleWrap();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void drawRosePattern(float wallX, float wallHeight) {
    glColor3f(0.95f, 0.82f, 0.1f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glVertex3f(wallX + 0.01f, 1.5f, -1.8f);
    glVertex3f(wallX + 0.01f, 2.5f,  0.5f);
    glVertex3f(wallX + 0.01f, 3.2f, -0.8f);
    glVertex3f(wallX + 0.01f, 0.8f,  2.0f);
    glVertex3f(wallX + 0.01f, 4.0f,  1.8f);
    glEnd();
    glPointSize(1.0f);

    glColor4f(0.85f, 0.08f, 0.08f, 0.8f);
    glPushMatrix();
    glTranslatef(wallX + 0.01f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    drawSolidCircle(-1.0f, 2.2f, 0.22f, 12);
    drawSolidCircle( 1.0f, 1.5f, 0.25f, 12);
    glPopMatrix();
}
