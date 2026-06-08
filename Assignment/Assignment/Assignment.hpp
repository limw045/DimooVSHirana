#ifndef ASSIGNMENT_HPP
#define ASSIGNMENT_HPP

#include "CGLabmain.hpp"
#include <string>
#include <vector>

namespace Assignment {

class MyVirtualWorld
{
 public:
    long int timeold, timenew, elapseTime;

    void draw();
    void tickTime();
    void init();
};

}; //end of namespace Assignment

#endif //ASSIGNMENT_HPP
