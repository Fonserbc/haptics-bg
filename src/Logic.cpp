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
    timePlayed = 0.0;
    minPos = minSpherePos;
    maxPos = maxSpherePos;

    targetSize = sphere->getRadius() * 4;
    spawnNewTarget();

    gameOver = false;
    initialized = true;
}

void Logic::spawnNewTarget(bool reduceSize) {
    timeInTarget = 0.0;

    // Remove old target
    if (target != NULL) {
        target->remove();
    }

    if (reduceSize) {
        targetSize -= sphere->getRadius() * 0.25;

        if (targetSize <= sphere->getRadius()) {
            gameOver = true;
//            initialized = false;
            return;
        }
    }

    // Create a random position
    double x = ((double) (rand() % 4) * 0.05) * pow(-1.0, (double) (rand() % 2)) - 0.03;
    double y = ((double) (rand() % 4) * 0.05) * pow(-1.0, (double) (rand() % 2));
    double z = ((double) (rand() % 4) * 0.05) * pow(-1.0, (double) (rand() % 2));
    cVector3d position(x, y, z);
//    std::cout << "New target area at (" << position << ") with size " << targetSize << std::endl;

    // Spawn cube
    target = new Cube(world, position, targetSize, material, sphere->getShadowOffset() - 0.0001);
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
    cVector3d dir = sphere->getPos() - target->getPos();
    double dist = dir.length();

    if (dist <= (targetSize/2.0))
        return true;
    else
        return false;
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
