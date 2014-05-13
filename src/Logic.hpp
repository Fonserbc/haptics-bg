#ifndef LOGIC_HPP
#define LOGIC_HPP

#include "chai3d.h"
#include "Sphere.hpp"
#include "Cube.hpp"

class Logic {
public:
    Logic(cWorld* world, Sphere* sphere);

    void update (double deltaTime);
};

#endif // LOGIC_HPP
