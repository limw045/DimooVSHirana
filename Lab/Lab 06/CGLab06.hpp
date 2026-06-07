/*
 TCG6223 Computer Graphics
 CGLab06.hpp
 Objective: Header File for Lab06 (Outer Space Scene)
*/
#ifndef YP_CGLAB06_HPP
#define YP_CGLAB06_HPP

#include "CGLabmain.hpp"

namespace CGLab06 {

class MyPlanets
{
 public:
    void drawStars();
    void drawPlanets();
};

class Spaceship
{
 public:
    void draw();
};

class MyVirtualWorld
{
 public:
    MyPlanets myplanets;
    Spaceship spaceship;

    void draw()
    {
        // Renders the background stars
        myplanets.drawStars();

        // Renders Jupiter, Saturn and Mars
        myplanets.drawPlanets();

        // Renders the spaceship hovering in the center
        glPushMatrix();
            glTranslatef(0.0f, -3.0f, 0.0f);
            glRotatef(-30.0f, 0.0f, 1.0f, 0.0f); // Face slightly to the right
            glRotatef(15.0f, 1.0f, 0.0f, 0.0f);  // Tilt nose up slightly
            glRotatef(10.0f, 0.0f, 0.0f, 1.0f);  // Roll slightly
            spaceship.draw();
        glPopMatrix();
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

}; //end of namespace CGLab06

#endif //YP_CGLAB06_HPP
