#ifndef LOGIC_HPP
#define LOGIC_HPP

#include "chai3d.h"
#include "Sphere.hpp"
#include "Cube.hpp"

class Logic {
public:
    Logic(cWorld* world, Sphere* sphere);

    void update(double deltaTime);
    void spawnNewTarget(bool reduceSize = false);
    bool sphereInTarget();

private:
    cWorld* world;
    Sphere* sphere;

    Cube* target;
    cMaterial material;
    double targetSize;
    double timeInTarget;
    double successTime;
};

#endif // LOGIC_HPP
