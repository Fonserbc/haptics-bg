#ifndef LOGIC_HPP
#define LOGIC_HPP

#include "chai3d.h"
#include "Sphere.hpp"
#include "Cube.hpp"

class Logic {
public:
    Logic(cWorld* world, Sphere* sphere);

    void init(cVector3d minSpherePos, cVector3d maxSpherePos);
    bool isReady();

    void update(double deltaTime);
    void spawnNewTarget(bool reduceSize = false);
    bool sphereInTarget();
    Cube* getTarget();

    double playTime();
    bool gameIsOver();

private:
    bool initialized;

    float step;

    cWorld* world;
    Sphere* sphere;

    Cube* target;
    cMaterial material, material2;
    double targetSize;
    double timeInTarget;
    double successTime;

    double timePlayed;
    double gameOver;

    cVector3d centerOfMovement, rangeOfMovement;

    cVector3d getRandomPosition();
};

#endif // LOGIC_HPP
