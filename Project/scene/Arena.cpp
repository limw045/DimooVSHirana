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

    // 初始化气泡纸 (8x6 阵列)
    float startX = -BOX_WIDTH / 2.0f + 0.5f;
    float startZ = -BOX_DEPTH / 2.0f + 0.5f;
    for (int r = 0; r < 6; ++r) {
        for (int c = 0; c < 8; ++c) {
            BubbleCell cell;
            cell.x = startX + c * 0.25f;
            cell.z = startZ + r * 0.25f;
            cell.pressDepth = 0.0f;
            cell.pressVelocity = 0.0f;
            cell.popped = false;
            bubbles.push_back(cell);
        }
    }
}

Arena::~Arena() {
    if (cardboardTex) glDeleteTextures(1, &cardboardTex);
    if (woodTex) glDeleteTextures(1, &woodTex);
    if (roseLabelTex) glDeleteTextures(1, &roseLabelTex);
    if (starrySkyTex) glDeleteTextures(1, &starrySkyTex);
}

void Arena::init() {
    // 加载纹理文件
    cardboardTex = loadTextureHelper("images/cardboard.jpg");
    woodTex = loadTextureHelper("images/wood.jpg");
    roseLabelTex = loadTextureHelper("images/rose_label.jpg");
    starrySkyTex = loadTextureHelper("images/starry_sky.jpg");
}

void Arena::update(float dt) {
    // 1. 物理计算：盒盖震动回弹
    float spring = -lid.springK * (lid.angle - lid.restAngle);
    float damp = -lid.damping * lid.velocity;
    lid.velocity += (spring + damp) * dt;
    lid.angle += lid.velocity * dt;
    lid.angle = clamp(lid.angle, 90.0f, 180.0f); // 物理限位，最大平开 180 度

    // 2. 物理计算：左右墙壁震动衰减
    if (leftWallShakeTime > 0.0f) {
        leftWallShakeTime -= dt;
        leftWallOffset = exp(-4.0f * (0.5f - leftWallShakeTime)) * sin(25.0f * leftWallShakeTime) * 0.12f;
        if (leftWallShakeTime <= 0.0f) {
            leftWallOffset = 0.0f;
        }
    }
    if (rightWallShakeTime > 0.0f) {
        rightWallShakeTime -= dt;
        rightWallOffset = exp(-4.0f * (0.5f - rightWallShakeTime)) * sin(25.0f * rightWallShakeTime) * 0.12f;
        if (rightWallShakeTime <= 0.0f) {
            rightWallOffset = 0.0f;
        }
    }

    // 3. 物理计算：气泡纸单元物理
    for (size_t i = 0; i < bubbles.size(); ++i) {
        if (bubbles[i].popped) continue;
        float bSpring = -25.0f * bubbles[i].pressDepth;
        float bDamp = -6.0f * bubbles[i].pressVelocity;
        bubbles[i].pressVelocity += (bSpring + bDamp) * dt;
        bubbles[i].pressDepth += bubbles[i].pressVelocity * dt;
        bubbles[i].pressDepth = clamp(bubbles[i].pressDepth, 0.0f, 1.0f);
    }

    // 4. 物理计算：说明书纸片飞行物理
    if (pamphlet.isFlying) {
        pamphlet.x += pamphlet.vx * dt;
        pamphlet.y += pamphlet.vy * dt;
        pamphlet.z += pamphlet.vz * dt;
        pamphlet.vy -= 4.0f * dt; // 重力加速度

        pamphlet.rotX += pamphlet.rotVX * dt;
        pamphlet.rotY += pamphlet.rotVY * dt;
        pamphlet.rotZ += pamphlet.rotVZ * dt;

        // 逐渐展开
        pamphlet.foldAngle += 0.8f * dt;
        if (pamphlet.foldAngle > 1.0f) pamphlet.foldAngle = 1.0f;

        // 地面碰撞检测 (Y轴高度 0.01)
        if (pamphlet.y <= 0.01f) {
            pamphlet.y = 0.01f;
            pamphlet.vy = 0.0f;
            pamphlet.vx = 0.0f;
            pamphlet.vz = 0.0f;
            pamphlet.rotVX = pamphlet.rotVY = pamphlet.rotVZ = 0.0f;
            // 随机平躺角度
            pamphlet.rotX = 0.0f;
            pamphlet.rotZ = 0.0f;
            pamphlet.isFlying = false;
        }
    }

    // 5. 物理计算：干燥剂包滑动
    float speedSq = desiccant.vx * desiccant.vx + desiccant.vz * desiccant.vz;
    if (speedSq > 0.0001f) {
        float speed = sqrt(speedSq);
        float friction = 1.5f; // 阻力
        desiccant.vx -= (desiccant.vx / speed) * friction * dt;
        desiccant.vz -= (desiccant.vz / speed) * friction * dt;

        desiccant.x += desiccant.vx * dt;
        desiccant.z += desiccant.vz * dt;

        // 边界碰撞检测
        float limitX = BOX_WIDTH / 2.0f - 0.2f;
        float limitZ = BOX_DEPTH / 2.0f - 0.2f;
        if (desiccant.x > limitX) { desiccant.x = limitX; desiccant.vx *= -0.5f; }
        if (desiccant.x < -limitX) { desiccant.x = -limitX; desiccant.vx *= -0.5f; }
        if (desiccant.z > limitZ) { desiccant.z = limitZ; desiccant.vz *= -0.5f; }
        if (desiccant.z < -limitZ) { desiccant.z = -limitZ; desiccant.vz *= -0.5f; }
    }
}

void Arena::triggerWallShake(float intensity, bool leftWall) {
    if (leftWall) {
        leftWallShakeTime = 0.5f; // 震动持续 0.5 秒
    } else {
        rightWallShakeTime = 0.5f;
    }
}

void Arena::triggerLidShake(float intensity) {
    lid.velocity += intensity;
}

void Arena::testExplodeProps() {
    // 强制引爆说明书
    if (!pamphlet.isFlying) {
        pamphlet.isFlying = true;
        pamphlet.y = 0.2f;
        pamphlet.vy = 2.0f + (rand() % 100) / 100.0f;
        pamphlet.vx = ((rand() % 200) / 100.0f - 1.0f) * 1.5f;
        pamphlet.vz = ((rand() % 200) / 100.0f - 1.0f) * 1.5f;
        pamphlet.rotVX = 360.0f;
        pamphlet.rotVY = 180.0f;
        pamphlet.rotVZ = 90.0f;
        pamphlet.foldAngle = 0.0f;
    }
    // 给干燥剂一个滑动冲量
    desiccant.vx = ((rand() % 200) / 100.0f - 1.0f) * 4.0f;
    desiccant.vz = ((rand() % 200) / 100.0f - 1.0f) * 4.0f;
    desiccant.popped = false;
}

void Arena::resolveCollisionWithProps(float charX, float charZ, float radius, float& outPushX, float& outPushZ) {
    outPushX = 0.0f;
    outPushZ = 0.0f;

    // 1. 气泡纸的压力踩扁判定
    for (size_t i = 0; i < bubbles.size(); ++i) {
        if (bubbles[i].popped) continue;
        float dx = bubbles[i].x - charX;
        float dz = bubbles[i].z - charZ;
        float distSq = dx * dx + dz * dz;
        if (distSq < radius * radius) {
            bubbles[i].pressDepth = 0.8f;
            bubbles[i].pressVelocity = -3.0f;
            
            // 15% 概率踩爆圆球
            if ((rand() % 100) < 2) {
                bubbles[i].popped = true;
            }
        }
    }

    // 2. 干燥剂的碰撞反推物理
    float dx = desiccant.x - charX;
    float dz = desiccant.z - charZ;
    float dist = sqrt(dx*dx + dz*dz);
    float minDist = radius + 0.2f; // 干燥剂包包围半径 0.2f
    if (dist < minDist && dist > 0.001f) {
        float push = minDist - dist;
        // 把干燥剂推开
        desiccant.x += (dx / dist) * push;
        desiccant.z += (dz / dist) * push;
        // 传递一部分动能
        desiccant.vx += (dx / dist) * 2.0f;
        desiccant.vz += (dz / dist) * 2.0f;
    }
}

// 纹理四边形绘制辅助方法
void Arena::drawTexturedQuad(float x1, float y1, float z1,
                             float x2, float y2, float z2,
                             float x3, float y3, float z3,
                             float x4, float y4, float z4,
                             float uMax, float vMax) {
    glBegin(GL_QUADS);
    // 逆时针绘制
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

    // 启用纹理
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cardboardTex);

    // --- 1. 盒底 (地板) ---
    GLfloat mat_floor_specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_floor_specular);
    glColor3f(0.97f, 0.96f, 0.94f); // 暖白色纸板色
    glNormal3f(0.0f, 1.0f, 0.0f);
    drawTexturedQuad(-halfW, 0.0f,  halfD,
                      halfW, 0.0f,  halfD,
                      halfW, 0.0f, -halfD,
                     -halfW, 0.0f, -halfD, 2.0f, 2.0f);

    // 禁用纹理，以便绘制盒壁内侧的图案
    glDisable(GL_TEXTURE_2D);

    // --- 2. 左壁 (小野的主场 - 深绿底 + 玫瑰/星光装饰) ---
    glPushMatrix();
    glTranslatef(leftWallOffset, 0, 0); // 震动偏移
    // 墙壁实体
    glBegin(GL_QUADS);
    glColor3f(0.15f, 0.38f, 0.22f); // 深绿色底色
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-halfW, 0.0f,   halfD);
    glVertex3f(-halfW, 0.0f,  -halfD);
    glVertex3f(-halfW, height, -halfD);
    glVertex3f(-halfW, height,  halfD);
    glEnd();

    // 绘制左壁的程序化图案 (星星和简化玫瑰)
    glDisable(GL_LIGHTING);
    drawRosePattern(-halfW, height);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    // --- 3. 右壁 (Dimoo的主场 - 浅蓝底 + 藤蔓/蝴蝶装饰) ---
    glPushMatrix();
    glTranslatef(rightWallOffset, 0, 0); // 震动偏移
    // 墙壁实体
    glBegin(GL_QUADS);
    glColor3f(0.85f, 0.93f, 0.96f); // 浅蓝底色
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(halfW, 0.0f,  -halfD);
    glVertex3f(halfW, 0.0f,   halfD);
    glVertex3f(halfW, height,  halfD);
    glVertex3f(halfW, height, -halfD);
    glEnd();

    // 绘制右壁的程序化图案 (藤蔓花纹)
    glDisable(GL_LIGHTING);
    // 右内壁程序化花纹 (绘制藤蔓装饰线段)
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

    // --- 4. 后壁 (盲盒介绍背面 - 贴纸和产品规格信息) ---
    glBegin(GL_QUADS);
    glColor3f(0.92f, 0.90f, 0.88f); // 纸底白灰色
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-halfW, 0.0f,  -halfD);
    glVertex3f( halfW, 0.0f,  -halfD);
    glVertex3f( halfW, height, -halfD);
    glVertex3f(-halfW, height, -halfD);
    glEnd();

    // 绘制后内壁信息 (模拟印刷文字条和条形码)
    glDisable(GL_LIGHTING);
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    // 模拟条形码背景
    glVertex3f(-1.5f, 0.8f, -halfD + 0.01f);
    glVertex3f(-0.5f, 0.8f, -halfD + 0.01f);
    glVertex3f(-0.5f, 1.8f, -halfD + 0.01f);
    glVertex3f(-1.5f, 1.8f, -halfD + 0.01f);
    glEnd();

    // 绘制条形码斑马线
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

    // 盒盖绕后壁顶边铰接
    glPushMatrix();
    glTranslatef(0.0f, height, -halfD);    // 移动到铰接中心
    glRotatef(lid.angle, 1.0f, 0.0f, 0.0f); // 绕X轴旋转
    glTranslatef(0.0f, 0.0f, halfD);        // 恢复坐标系偏移

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cardboardTex);

    // 渲染折叠盒盖
    glColor3f(0.82f, 0.76f, 0.70f); // 牛皮纸黄色
    glNormal3f(0.0f, 1.0f, 0.0f);
    // 盒盖也是一个 6x6 的矩形面片
    drawTexturedQuad(-halfW, 0.0f,  halfD,
                      halfW, 0.0f,  halfD,
                      halfW, 0.0f, -halfD,
                     -halfW, 0.0f, -halfD, 1.0f, 1.0f);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void Arena::drawBubbleWrap() {
    // 渲染气泡纸底板
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE); // 属于半透明物件，暂时关闭深度写入

    // 气泡纸半透明浅绿底片
    glColor4f(0.75f, 0.92f, 0.78f, 0.4f);
    float halfW = 1.0f;
    float halfD = 0.75f;
    float posX = -BOX_WIDTH / 2.0f + 1.2f;
    float posZ = -BOX_DEPTH / 2.0f + 1.2f;

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(posX - halfW, 0.005f, posZ + halfD);
    glVertex3f(posX + halfW, 0.005f, posZ + halfD);
    glVertex3f(posX + halfW, 0.005f, posZ - halfD);
    glVertex3f(posX - halfW, 0.005f, posZ - halfD);
    glEnd();

    // 绘制气泡突起圆球
    glColor4f(0.85f, 0.97f, 0.88f, 0.65f);
    GLfloat mat_bubble_shininess = 60.0f;
    GLfloat mat_bubble_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_bubble_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_bubble_shininess);

    for (size_t i = 0; i < bubbles.size(); ++i) {
        if (bubbles[i].popped) continue;
        glPushMatrix();
        glTranslatef(bubbles[i].x, 0.01f, bubbles[i].z);
        // 根据踩中深度缩放 Y 轴
        glScalef(1.0f, 0.7f * (1.0f - bubbles[i].pressDepth * 0.8f), 1.0f);
        glutSolidSphere(0.07, 8, 8);
        glPopMatrix();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void Arena::drawPamphlet() {
    // 绘制说明书
    glPushMatrix();
    glTranslatef(pamphlet.x, pamphlet.y, pamphlet.z);
    glRotatef(pamphlet.rotX, 1, 0, 0);
    glRotatef(pamphlet.rotY, 0, 1, 0);
    glRotatef(pamphlet.rotZ, 0, 0, 1);

    // 左右对折效果
    glColor3f(0.95f, 0.95f, 0.95f); // 纯白纸色
    
    // 左半片
    glPushMatrix();
    glRotatef((1.0f - pamphlet.foldAngle) * 35.0f, 0, 0, 1);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.3f, 0.0f,  0.4f);
    glVertex3f( 0.0f, 0.0f,  0.4f);
    glVertex3f( 0.0f, 0.0f, -0.4f);
    glVertex3f(-0.3f, 0.0f, -0.4f);
    glEnd();
    glPopMatrix();

    // 右半片
    glPushMatrix();
    glRotatef((1.0f - pamphlet.foldAngle) * -35.0f, 0, 0, 1);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f( 0.0f, 0.0f,  0.4f);
    glVertex3f( 0.3f, 0.0f,  0.4f);
    glVertex3f( 0.3f, 0.0f, -0.4f);
    glVertex3f( 0.0f, 0.0f, -0.4f);
    glEnd();
    glPopMatrix();

    glPopMatrix();
}

void Arena::drawDesiccant() {
    // 绘制干燥剂包 (长方体)
    glPushMatrix();
    glTranslatef(desiccant.x, desiccant.y, desiccant.z);
    glRotatef(12.0f, 0.0f, 1.0f, 0.0f); // 随机偏斜一点

    glColor3f(0.90f, 0.90f, 0.90f); // 包装白无纺布色
    
    // 手工绘制一个长方体 (0.35 x 0.1 x 0.25)
    float w = 0.175f, h = 0.05f, d = 0.125f;
    glBegin(GL_QUADS);
    // 顶
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-w, h,  d); glVertex3f( w, h,  d);
    glVertex3f( w, h, -d); glVertex3f(-w, h, -d);
    // 底
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(-w, -h, -d); glVertex3f( w, -h, -d);
    glVertex3f( w, -h,  d); glVertex3f(-w, -h,  d);
    // 前面
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-w, -h,  d); glVertex3f( w, -h,  d);
    glVertex3f( w,  h,  d); glVertex3f(-w,  h,  d);
    // 后面
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f( w, -h, -d); glVertex3f(-w, -h, -d);
    glVertex3f(-w,  h, -d); glVertex3f( w,  h, -d);
    glEnd();

    glPopMatrix();
}

void Arena::drawOpaque() {
    drawBox();
    drawLid();
    drawPamphlet();
    drawDesiccant();
}

void Arena::drawTransparent() {
    // 1. 绘制半透明前壁
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE); // 关闭深度写入

    float halfW = BOX_WIDTH / 2.0f;
    float halfD = BOX_DEPTH / 2.0f;
    float height = BOX_HEIGHT;

    // 前壁 (朝向摄像机，高透明度)
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

    glDepthMask(GL_TRUE); // 重新开启深度写入
    glDisable(GL_BLEND);
}

// 辅助方法：绘制左内壁图案的玫瑰花/星光图案
void drawRosePattern(float wallX, float wallHeight) {
    // 1. 绘制金色星光 (程序化圆点)
    glColor3f(0.95f, 0.82f, 0.1f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    // 固定坐标，使之不会闪烁
    glVertex3f(wallX + 0.01f, 1.5f, -1.8f);
    glVertex3f(wallX + 0.01f, 2.5f,  0.5f);
    glVertex3f(wallX + 0.01f, 3.2f, -0.8f);
    glVertex3f(wallX + 0.01f, 0.8f,  2.0f);
    glVertex3f(wallX + 0.01f, 4.0f,  1.8f);
    glEnd();
    glPointSize(1.0f);

    // 2. 绘制玫瑰剪影
    glColor4f(0.85f, 0.08f, 0.08f, 0.8f);
    // 将 3D 平面贴合在 X 坐标为 wallX 的面上，旋转对齐
    glPushMatrix();
    glTranslatef(wallX + 0.01f, 0.0f, 0.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f); // 对准左侧壁面

    // 玫瑰花瓣大圆 (以Z, Y为平面坐标系画圆)
    drawSolidCircle(-1.0f, 2.2f, 0.22f, 12);
    drawSolidCircle( 1.0f, 1.5f, 0.25f, 12);

    glPopMatrix();
}
