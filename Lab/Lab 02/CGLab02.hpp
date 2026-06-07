/*
 TCS2221 Computer Graphics
 CGLab02.hpp
 Objective: Header File for Lab02 on triangles, quads and polygons
*/
#ifndef YP_CGLAB02_HPP
#define YP_CGLAB02_HPP

#include "CGLabmain.hpp"

namespace CGLab02 {

class MyTriangles
{
 public:
    void draw();
};

class MyRoom
{
 public:
    void draw();
    void drawWalls();
    void drawTable();
    void drawDustbin();
    void drawShelves();
};

class MyVirtualWorld
{
 public:
    MyTriangles mytriangles;
    MyRoom      myroom;

    void draw()
    {
        mytriangles.draw();
        myroom.draw();
    }

    void tickTime()
    {
        // do nothing, reserved for doing animation
    }

    void init()
    {
        // for any one-time only initialization of the
        // virtual world before any rendering takes place
        // BUT after OpenGL has been initialized
    }
};

}; //end of namespace CGLab02

#endif //YP_CGLAB02_HPP
