/*
TCG6223 Computer Graphics
Trimester March/April 2026 (Term 2610)
CGLabSimpleFlower.hpp
Objective: Header File for Simple Flower Lab Assignment
*/

#ifndef YP_CGLAB01_HPP
#define YP_CGLAB01_HPP

#include "CGLabmain.hpp"
#include <string>
#include <vector>

namespace CGLabSimpleFlower {

class SimpleFlower
{
public:
    void drawCore();
    void drawPetal();
    void drawPetals();
};

//------------------------------------
//the main program will call methods from this class
class MyVirtualWorld
{
public:
    SimpleFlower simpleFlower;
    long int timeold, timenew, elapseTime;

    void draw() {
        simpleFlower.drawCore();
        simpleFlower.drawPetals();
    }

    void tickTime()
    {
    }

    //for any one-time only initialization of the
    // virtual world before any rendering takes place
    // BUT after OpenGL has been initialized
    void init()
    {
    }
};

}; //end of namespace CGLabSimpleFlower

#endif //YP_CGLAB01_HPP
