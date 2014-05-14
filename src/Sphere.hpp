#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "chai3d.h"
#include "Cube.hpp"

class Sphere {
public:
    Sphere (cWorld* world, double radius, cMaterial material, double shadowOffset);

    void setPos(cVector3d pos);
    cVector3d getPos();
    double getRadius();
    double getShadowOffset();

private:
    cWorld* world;
    Cube* room;

    double radius, shadowOffset;

    cShapeSphere* sphere;
    cMesh* shadow;
};

#endif // SPHERE_HPP
