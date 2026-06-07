#include "Common.hpp"
#include "Game.h"
#include <iostream>



// 全局游戏控制对象
Game game;

// 计时变量
static float lastTime = 0.0f;

// 渲染回调
void display() {
    game.draw();
}

// 物理与逻辑状态更新回调
void idle() {
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    float dt = currentTime - lastTime;
    lastTime = currentTime;

    // 限制最大 dt，防止大跨度卡顿时物体飞出边界 (防止跳跃或物理公式发散)
    if (dt > 0.05f) {
        dt = 0.05f;
    }

    game.update(dt);
    glutPostRedisplay(); // 强行重绘屏幕
}

// 窗口尺寸缩放回调
void reshape(int width, int height) {
    if (height == 0) height = 1;
    float aspect = (float)width / (float)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, aspect, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

// 键盘普通按键按下
void keyboard(unsigned char key, int x, int y) {
    game.handleInput(key);
}

// 特殊按键 (方向键、功能键) 按下
void special(int key, int x, int y) {
    game.handleSpecialInput(key);
}


int main(int argc, char** argv) {
    // 1. 初始化 GLUT 窗口
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Blind Box Battle - Stage 1 (Empty Box Scene)");

    // 2. 初始化游戏资源与状态
    game.init();

    // 3. 注册 GLUT 回调函数
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);

    // 注册键盘按下回调
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);



    // 4. 初始化全局 OpenGL 渲染特性
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glShadeModel(GL_SMOOTH);
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f); // 极暗背景色，反衬出亮色的纸箱
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // 输出游戏操作引导
    std::cout << "==========================================================" << std::endl;
    std::cout << "           BLIND BOX BATTLE: STAGE 1 SCENE STARTED        " << std::endl;
    std::cout << "==========================================================" << std::endl;
    std::cout << "[Controls]" << std::endl;
    std::cout << "  - P1 (Hirono Dummy): [W][A][S][D] to slide" << std::endl;
    std::cout << "  - P2 (Dimoo Dummy) : [ARROWS] to slide" << std::endl;
    std::cout << "  - Open Boxes       : Press [SPACE] or [ENTER]" << std::endl;
    std::cout << "  - Restart          : Press [R]" << std::endl;
    std::cout << "[Debug Keys]" << std::endl;
    std::cout << "  - F4 : Toggle collision cylinder wireframes" << std::endl;
    std::cout << "  - F5 : Skip entry unboxing animation directly to battle" << std::endl;
    std::cout << "  - F6 : Teleport P1 to left wall (triggers impact shake)" << std::endl;
    std::cout << "  - F7 : Shakes the cardboard box lid (triggers ultimate shake)" << std::endl;
    std::cout << "  - [H] : Kick the silica gel packet" << std::endl;
    std::cout << "  - [P] : Launch the folded instructions leaflet" << std::endl;
    std::cout << "  - [E] : Shake left wall" << std::endl;
    std::cout << "==========================================================" << std::endl;

    // 5. 进入主循环
    glutMainLoop();
    return 0;
}
