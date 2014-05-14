#include <math.h>
#include "Logic.hpp"

Logic::Logic(cWorld* world, Sphere* sphere) {
    this->world = world;
    this->sphere = sphere;

    material.m_ambient.set(0.1, 0.1, 0.4);
    material.m_diffuse.set(0.3, 0.3, 0.8);
    material.m_specular.set(1.0, 1.0, 1.0);

    target = NULL;
    targetSize = sphere->getRadius() * 4;
    successTime = 2.0;

    srand (time(NULL));
    spawnNewTarget();
}

void Logic::spawnNewTarget(bool reduceSize) {
    timeInTarget = 0.0;

    // Remove old target
    if (target != NULL) {
        target->remove();
        delete target;
    }

    if (reduceSize)
        targetSize -= sphere->getRadius() * 0.25;

    // Create a random position
    double x = ((double) (rand() % 4) * 0.05) * pow(-1.0, (double) (rand() % 2)) - 0.03;
    double y = ((double) (rand() % 4) * 0.05) * pow(-1.0, (double) (rand() % 2));
    double z = ((double) (rand() % 4) * 0.05) * pow(-1.0, (double) (rand() % 2));
    cVector3d position(x, y, z);

    target = new Cube(world, position, targetSize, material);
}

void Logic::update(double deltaTime) {
    if(sphereInTarget()) {
        if (timeInTarget >= successTime)
            spawnNewTarget(true);

        timeInTarget += deltaTime;
    } else {
        timeInTarget = 0.0;
    }
}

bool Logic::sphereInTarget() {
    cVector3d dir = sphere->getPos() - target->getPos();
    double dist = dir.length();

    if (dist + sphere->getRadius() <= (targetSize/2.0))
        return true;
    else
        return false;
}
