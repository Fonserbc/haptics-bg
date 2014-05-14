#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "chai3d.h"
#include "Cube.hpp"

class Sphere {
public:
    Sphere (cWorld* world, Cube* room, double radius, cMaterial material);

    void setPos(cVector3d pos);
    cVector3d getPos();
    double getRadius();

private:
    cWorld* world;
    Cube* room;

    double radius;

    cShapeSphere* sphere;
    cMesh* shadow;
};

#endif // SPHERE_HPP
