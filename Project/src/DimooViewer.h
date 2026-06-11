#ifndef DIMOO_VIEWER_H
#define DIMOO_VIEWER_H

#include "Common.hpp"

namespace DimooViewerApp {

void init();
void shutdown();
void display();
void reshape(int width, int height);
void idle();
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void special(int key, int x, int y);
void specialUp(int key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void passiveMotion(int x, int y);

}

#endif
