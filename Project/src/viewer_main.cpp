#include "Common.hpp"
#include "DimooViewer.h"

#include <iostream>

int main(int argc, char** argv) {
    glutInit(&argc, argv);
#ifdef __FREEGLUT_EXT_H__
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
#endif
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(1400, 900);
    glutInitWindowPosition(120, 80);
    glutCreateWindow("Dimoo Model Viewer");

    DimooViewerApp::init();

    glutDisplayFunc(DimooViewerApp::display);
    glutReshapeFunc(DimooViewerApp::reshape);
    glutIdleFunc(DimooViewerApp::idle);
    glutKeyboardFunc(DimooViewerApp::keyboard);
    glutSpecialFunc(DimooViewerApp::special);
    glutMouseFunc(DimooViewerApp::mouse);
    glutMotionFunc(DimooViewerApp::motion);
    glutPassiveMotionFunc(DimooViewerApp::passiveMotion);

    std::cout << "========================================" << std::endl;
    std::cout << "         DIMOO MODEL VIEWER READY       " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "ESC : close viewer" << std::endl;
    std::cout << "SPACE : pause/resume animation" << std::endl;
    std::cout << "F : flip facing" << std::endl;
    std::cout << "T : toggle face texture" << std::endl;
    std::cout << "R : reset viewer state" << std::endl;

    glutMainLoop();
    DimooViewerApp::shutdown();
    return 0;
}
