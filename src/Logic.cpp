#include <math.h>
#include "Logic.hpp"

Logic::Logic(cWorld* world, Sphere* sphere) {
    this->world = world;
    this->sphere = sphere;

    material.m_ambient.set(0.1, 0.1, 0.4);
    material.m_diffuse.set(0.3, 0.3, 0.8);
    material.m_specular.set(1.0, 1.0, 1.0);

    material2.m_ambient.set(0.4, 0.1, 0.1);
    material2.m_diffuse.set(0.8, 0.3, 0.3);
    material2.m_specular.set(1.0, 1.0, 1.0);

    target = NULL;
    successTime = 2.0;

    srand(time(NULL));

    initialized = false;
}

void Logic::init(cVector3d minSpherePos, cVector3d maxSpherePos) {
    // Get the equilibrium between the minimum and maxium possible position of the sphere
    centerOfMovement = (maxSpherePos + minSpherePos) * 0.5;
    // For every axis get the limits of movement
    rangeOfMovement.x = cMin(maxSpherePos.x - centerOfMovement.x, centerOfMovement.x - minSpherePos.x);
    rangeOfMovement.y = cMin(maxSpherePos.y - centerOfMovement.y, centerOfMovement.y - minSpherePos.y);
    rangeOfMovement.z = cMin(maxSpherePos.z - centerOfMovement.z, centerOfMovement.z - minSpherePos.z);
    rangeOfMovement *= 0.9;

    targetSize = sphere->getRadius() * 2.5;

    float tests = 10.0;
    step = 2.5/tests;

    spawnNewTarget();

    timePlayed = 0.0;
    gameOver = false;
    initialized = true;
}

void Logic::spawnNewTarget(bool reduceSize) {
    timeInTarget = 0.0;

    // Remove old target
    if (target != NULL)
        target->remove();

    // Reduce size of target area
    if (reduceSize) {
        targetSize -= sphere->getRadius() * step;

        if (targetSize <= sphere->getRadius()) {
            gameOver = true;
//            initialized = false;
            return;
        }
    }

    // Spawn cube
//    std::cout << "New target area at (" << position << ") with size " << targetSize << std::endl;
    target = new Cube(world, getRandomPosition(), targetSize, material, sphere->getShadowOffset() - 0.0001);
}

cVector3d Logic::getRandomPosition() {
    //                    -1 or 1                              [0,1]                    maximum offset to center of movement
    double x = pow(-1.0, (double) (rand() % 2)) * ((double) (rand() % 21) * 0.05) * ((rangeOfMovement.x) - sphere->getRadius()) + centerOfMovement.x;
    double y = pow(-1.0, (double) (rand() % 2)) * ((double) (rand() % 21) * 0.05) * ((rangeOfMovement.y) - sphere->getRadius()) + centerOfMovement.y;
    double z = pow(-1.0, (double) (rand() % 2)) * ((double) (rand() % 21) * 0.05) * ((rangeOfMovement.z) - sphere->getRadius()) + centerOfMovement.z;
    return cVector3d(x, y, z);
}

void Logic::update(double deltaTime) {
    timePlayed += deltaTime;

    if(sphereInTarget()) {
        if (timeInTarget >= successTime)
            spawnNewTarget(true);

        timeInTarget += deltaTime;
        target->setMaterial(material2);
    } else {
        timeInTarget = 0.0;
        target->setMaterial(material);
    }
}

bool Logic::sphereInTarget() {
    double dist = sphere->getPos().distance(target->getPos());

    if (dist <= (targetSize/2.0))
        return true;
    else
        return false;
}


Cube* Logic::getTarget() {
    return target;
}

bool Logic::isReady() {
    return initialized;
}

double Logic::playTime() {
    return timePlayed;
}

bool Logic::gameIsOver() {
    return gameOver;
}
